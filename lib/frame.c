/*
 * \brief  MTK Frame widget module
 */

/*
 * Copyright (C) 2002-2008 Norman Feske <norman.feske@genode-labs.com>
 * Genode Labs, Feske & Helmuth Systementwicklung GbR
 *
 * This file is part of the MTK package, which is distributed
 * under the terms of the GNU General Public License version 2.
 */

struct frame;
#define WIDGET struct frame

#include <stdio.h>
#include "keycodes.h"
#include "mtkstd.h"
#include "frame.h"
#include "script.h"
#include "widget_data.h"
#include "widget_help.h"
#include "scrollbar.h"
#include "background.h"
#include "gfx.h"
#include "widman.h"
#include "messenger.h"

static struct gfx_services        *gfx;
static struct script_services     *script;
static struct widman_services     *widman;
static struct scrollbar_services  *scroll;
static struct messenger_services  *msg;
static struct background_services *bg;

#define FRAME_MODE_SCRX 0x04    /* horizontal scrollbars               */
#define FRAME_MODE_SCRY 0x08    /* vertical scrollbars                 */
#define FRAME_MODE_HIDX 0x10    /* auto-hiding of horizontal scrollbar */
#define FRAME_MODE_HIDY 0x20    /* auto-hiding of vertical scrollbar   */
#define FRAME_MODE_BG   0x40    /* draw background                     */

struct frame_data {
	WIDGET     *content;   /* content of the frame       */
	u32         mode;      /* frame properties           */
	s32         scroll_x;  /* adaptive scroll x-position */
	s32         scroll_y;  /* adaptive scroll y-position */
	SCROLLBAR  *sb_x;      /* horizontal scrollbar       */
	SCROLLBAR  *sb_y;      /* vertical scrollbar         */
	BACKGROUND *corner;    /* corner between scrollbars  */
};

int init_frame(struct mtk_services *d);

static struct widget_methods gen_methods;


static inline int get_view_w(FRAME *f)
{
	return f->wd->w - (f->fd->sb_y ? 13 : 0);
}


static inline int get_view_h(FRAME *f)
{
	return f->wd->h - (f->fd->sb_x ? 13 : 0);
}


/****************************
 ** General widget methods **
 ****************************/

static int frame_draw(FRAME *f, struct gfx_ds *ds, int x, int y, WIDGET *origin)
{
	WIDGET *cw;
	s32 vw = get_view_w(f);
	s32 vh = get_view_h(f);
	int ret = 0;

	x += f->wd->x;
	y += f->wd->y;

	if (f == origin) return 1;

	gfx->push_clipping(ds, x, y, vw, vh);
	cw = f->fd->content;

	/* draw background if there is no or a non-concealing background */
	if (!cw || !(cw && cw->wd->flags & WID_FLAGS_CONCEALING)) {
		ret |= f->gen->draw_bg(f, ds, x, y, vw, vh, origin, 0);
	}

	/* if content exists, draw it */
	if (cw) ret |= cw->gen->draw(cw, ds, x, y, origin);
	gfx->pop_clipping(ds);

	if (f->fd->corner) ret |= f->fd->corner->gen->draw((WIDGET *)f->fd->corner, ds, x, y, origin);

	/* draw scrollbars */
	if (f->fd->sb_x)
		ret |= f->fd->sb_x->gen->draw((WIDGET *)f->fd->sb_x, ds, x, y, origin);
	if (f->fd->sb_y)
		ret |= f->fd->sb_y->gen->draw((WIDGET *)f->fd->sb_y, ds, x, y, origin);

	return ret;
}


static WIDGET *frame_find(FRAME *f, int x, int y)
{
	WIDGET *result;
	WIDGET *cw;

	/* check if position is inside the frame */
	if ((x >= f->wd->x) && (x < f->wd->x+f->wd->w)
	 && (y >= f->wd->y) && (y < f->wd->y+f->wd->h)) {

		/* we are hit - lets check our children */
		if (f->fd->sb_x) {
			result = f->fd->sb_x->gen->find((WIDGET *)f->fd->sb_x, x-f->wd->x, y-f->wd->y);
			if (result) return result;
		}
		if (f->fd->sb_y) {
			result = f->fd->sb_y->gen->find((WIDGET *)f->fd->sb_y, x-f->wd->x, y-f->wd->y);
			if (result) return result;
		}

		cw = f->fd->content;
		if (cw) {
			result = cw->gen->find(cw, x-f->wd->x, y-f->wd->y);
			if (result) return result;
		}
		return f;
	}
	return NULL;
}


/**
 * Calculate min/max size of frame - depending on scrollbar configuration
 */
static void frame_calc_minmax(FRAME *f)
{
	SCROLLBAR *sb;

	f->wd->min_w = f->wd->min_h = 0;
	if (f->fd->content) {
		f->wd->max_w = f->fd->content->gen->get_max_w(f->fd->content);
		f->wd->max_h = f->fd->content->gen->get_max_h(f->fd->content);
	} else {
		f->wd->max_w = f->wd->max_h = 99999;
	}
	
	if ((sb = f->fd->sb_y)) {
		int arrow_size = sb->scroll->get_arrow_size(sb);
		f->wd->min_w += arrow_size;
		f->wd->max_w += arrow_size;
		f->wd->min_h += arrow_size * 3;
	}

	if ((sb = f->fd->sb_x)) {
		int arrow_size = sb->scroll->get_arrow_size(sb);
		f->wd->min_w += arrow_size * 3;
		f->wd->min_h += arrow_size;
		f->wd->max_h += arrow_size;
	}
	if (f->wd->min_w > f->wd->max_w) f->wd->max_w = f->wd->min_w;
	if (f->wd->min_h > f->wd->max_h) f->wd->max_h = f->wd->min_h;
}


/**
 * Adapt position and size of children to frame size
 */
static void (*orig_updatepos)(WIDGET *w);
static void frame_updatepos(FRAME *f)
{
	WIDGET *c = f->fd->content;
	SCROLLBAR *sb = NULL;
	s32 vw, vh;

	vh = f->wd->h;
	if (f->fd->sb_x) vh -= 13;

	vw = f->wd->w;
	if (f->fd->sb_y) vw -= 13;

	if (c) {
		int cont_w = vw, cont_h = vh;
		if (cont_w > c->gen->get_max_w(c)) cont_w = c->gen->get_max_w(c);
		if (cont_w < c->gen->get_min_w(c)) cont_w = c->gen->get_min_w(c);
		if (cont_h > c->gen->get_max_h(c)) cont_h = c->gen->get_max_h(c);
		if (cont_h < c->gen->get_min_h(c)) cont_h = c->gen->get_min_h(c);

		if (f->fd->scroll_x > cont_w - vw) f->fd->scroll_x = cont_w - vw;
		if (f->fd->scroll_y > cont_h - vh) f->fd->scroll_y = cont_h - vh;
		if (f->fd->scroll_x < 0) f->fd->scroll_x = 0;
		if (f->fd->scroll_y < 0) f->fd->scroll_y = 0;

		c->gen->set_x(c, -f->fd->scroll_x);
		c->gen->set_y(c, -f->fd->scroll_y);
		c->gen->set_w(c, cont_w);
		c->gen->set_h(c, cont_h);
		c->gen->updatepos(c);
	}

	if ((sb = f->fd->sb_x)) {
		if (c) sb->scroll->set_real_size(sb, c->gen->get_w(c));
		sb->scroll->set_view_offset(sb, f->fd->scroll_x);
		sb->gen->set_x((WIDGET *)sb, 0);
		sb->gen->set_y((WIDGET *)sb, vh);
		sb->gen->set_w((WIDGET *)sb, vw);
		sb->gen->set_h((WIDGET *)sb, 13);
		sb->gen->updatepos((WIDGET *)sb);
	}

	if ((sb = f->fd->sb_y)) {
		if (c) sb->scroll->set_real_size(sb, c->gen->get_h(c));
		sb->scroll->set_view_offset(sb, f->fd->scroll_y);
		sb->gen->set_x((WIDGET *)sb, vw);
		sb->gen->set_y((WIDGET *)sb, 0);
		sb->gen->set_w((WIDGET *)sb, 13);
		sb->gen->set_h((WIDGET *)sb, vh);
		sb->gen->updatepos((WIDGET *)sb);
	}

	if (f->fd->corner) {
		f->fd->corner->gen->set_x((WIDGET *)f->fd->corner, vw-1);
		f->fd->corner->gen->set_y((WIDGET *)f->fd->corner, vh-1);
		f->fd->corner->gen->updatepos((WIDGET *)f->fd->corner);
	}

	orig_updatepos(f);

	/* send resize event */
	{
		char *m = f->gen->get_bind_msg(f, "resize");
		s32 id = f->gen->get_app_id(f);
		if (m) msg->send_action_event(id, "resized", m);
	}
}

static void frame_expose(FRAME *f, s32 x, s32 y);

static void (*orig_handle_event) (FRAME *f, EVENT *ev, WIDGET *from);
static void frame_handle_event(FRAME *f, EVENT *ev, WIDGET *from)
{
	if ((ev->type == EVENT_PRESS) && ((ev->code == MTK_BTN_GEAR_UP) || (ev->code == MTK_BTN_GEAR_DOWN))) {
		s32 vw = get_view_w(f)/2;
		s32 vh = get_view_h(f)/2;
		if(ev->code == MTK_BTN_GEAR_UP)
			frame_expose(f, f->fd->scroll_x+vw, f->fd->scroll_y+vh-35);
		else
			frame_expose(f, f->fd->scroll_x+vw, f->fd->scroll_y+vh+35);
	} else
		orig_handle_event(f, ev, from);
}

/**
 * Free frame widget data
 */
static void frame_free_data(FRAME *f)
{
	if (!f) return;
	if (f->fd->content) f->gen->release((WIDGET *)f->fd->content);
	if (f->fd->sb_x)    f->gen->release((WIDGET *)f->fd->sb_x);
	if (f->fd->sb_y)    f->gen->release((WIDGET *)f->fd->sb_y);
	if (f->fd->corner)  f->gen->release((WIDGET *)f->fd->corner);
}


/**
 * Return first child widget that can take the keyboard focus
 */
static WIDGET *frame_first_kfocus(FRAME *f)
{
	if (f->fd->content) return f->fd->content->gen->first_kfocus(f->fd->content);
	return NULL;
}


/**
 * Request widget type
 */
static char *frame_get_type(FRAME *f)
{
	return "Frame";
}


/**
 * Remove content from the frame
 */
static void frame_remove_child(FRAME *f, WIDGET *child)
{
	if (!child || (f->fd->content != child)) return;

	f->fd->content = NULL;
	child->gen->set_parent(child, NULL);
	child->gen->dec_ref(child);

	f->wd->update |= WID_UPDATE_MINMAX;
	f->gen->update(f);
}


/****************************
 ** Frame specific methods **
 ****************************/

static void frame_set_content(FRAME *f, WIDGET *new_content)
{
	/* abort if there is an cyclic parent relationship */
	if (new_content && new_content->gen->related_to(new_content, f)) return;

	if (f->fd->content) {
		f->fd->content->gen->set_parent(f->fd->content, NULL);
		f->fd->content->gen->dec_ref(f->fd->content);
	}
	f->fd->content = new_content;
	if (!new_content) return;
	
	new_content->gen->inc_ref(new_content);
	new_content->gen->release(new_content);
	new_content->gen->set_parent(new_content, f);

	new_content->gen->set_x(new_content, 0);
	new_content->gen->set_y(new_content, 0);
	new_content->gen->set_w(new_content, new_content->gen->get_min_w(new_content));
	new_content->gen->set_h(new_content, new_content->gen->get_min_h(new_content));
	new_content->gen->updatepos(new_content);

	f->fd->scroll_x = 0;
	f->fd->scroll_y = 0;
	f->wd->update |= WID_UPDATE_MINMAX;
}


static WIDGET *frame_get_content(FRAME *f)
{
	return f->fd->content;
}


static void frame_set_background(FRAME *f, int bg_flag)
{
	if (bg_flag) f->fd->mode |= FRAME_MODE_BG;
	else f->fd->mode &= ~FRAME_MODE_BG;
}


static int frame_get_background(FRAME *f)
{
	return !!(f->fd->mode & FRAME_MODE_BG);
}


static void frame_xscroll_update(FRAME *f)
{
	WIDGET *cw;
	if ((cw = f->fd->content) && f->fd->sb_x) {
		s32 new_scroll_x = f->fd->sb_x->scroll->get_view_offset(f->fd->sb_x);
		cw->gen->set_x(cw, cw->gen->get_x(cw) - new_scroll_x + f->fd->scroll_x);
		f->fd->scroll_x = new_scroll_x;
	}
	f->gen->force_redraw(f);
}


static void frame_yscroll_update(FRAME *f)
{
	WIDGET *cw;
	if ((cw = f->fd->content) && f->fd->sb_y) {
		s32 new_scroll_y = f->fd->sb_y->scroll->get_view_offset(f->fd->sb_y);
		cw->gen->set_y(cw, cw->gen->get_y(cw) - new_scroll_y + f->fd->scroll_y);
		f->fd->scroll_y = new_scroll_y;
	}
	f->gen->force_redraw(f);
}


static void check_corner(FRAME *f)
{
	if (f->fd->sb_x && f->fd->sb_y) {
		if (!f->fd->corner) f->fd->corner = bg->create();
		f->fd->corner->gen->set_w((WIDGET *)f->fd->corner, 15);
		f->fd->corner->gen->set_h((WIDGET *)f->fd->corner, 15);
	} else {
		if (f->fd->corner) {
			f->fd->corner->gen->dec_ref((WIDGET *)f->fd->corner);
			f->fd->corner = NULL;
		}
	}
}


static void frame_set_scrollx(FRAME *f, int flag)
{
	if (flag) {
		f->fd->mode |= FRAME_MODE_SCRX;
		if (!f->fd->sb_x) {
			f->fd->sb_x = scroll->create();
			f->fd->sb_x->gen->set_parent((WIDGET *)f->fd->sb_x, f);
			f->fd->sb_x->scroll->reg_scroll_update(f->fd->sb_x, (void (*)(void *))frame_xscroll_update, f);
			f->fd->sb_x->scroll->set_orient(f->fd->sb_x, "horizontal");
			f->fd->sb_x->gen->calc_minmax((WIDGET *)f->fd->sb_x);
		}
	} else {
		f->fd->mode &= ~FRAME_MODE_SCRX;
		if (f->fd->sb_x) {
			f->fd->sb_x->gen->dec_ref((WIDGET *)f->fd->sb_x);
			f->fd->sb_x = NULL;
		}
	}
	check_corner(f);
	f->wd->update |= WID_UPDATE_MINMAX;
}


static int frame_get_scrollx(FRAME *f)
{
	if (f->fd->mode & FRAME_MODE_SCRX) return 1;
	else return 0;
}


static void frame_set_scrolly(FRAME *f, int flag)
{
	if (flag) {
		f->fd->mode |= FRAME_MODE_SCRY;
		if (!f->fd->sb_y) {
			f->fd->sb_y = scroll->create();
			f->fd->sb_y->gen->set_parent((WIDGET *)f->fd->sb_y, f);
			f->fd->sb_y->scroll->reg_scroll_update(f->fd->sb_y, (void (*)(void *))frame_yscroll_update, f);
			f->fd->sb_y->scroll->set_orient(f->fd->sb_y, "vertical");
			f->fd->sb_y->gen->calc_minmax((WIDGET *)f->fd->sb_y);
		}
	} else {
		f->fd->mode &= ~FRAME_MODE_SCRY;
		if (f->fd->sb_y) {
			f->fd->sb_y->gen->dec_ref((WIDGET *)f->fd->sb_y);
			f->fd->sb_y = NULL;
		}
	}
	check_corner(f);
	f->wd->update |= WID_UPDATE_MINMAX;
}


static int frame_get_scrolly(FRAME *f)
{
	if (f->fd->mode & FRAME_MODE_SCRY) return 1;
	else return 0;
}

/**
 * Define position of the content within the frame
 *
 * It the specified child is not yet the content - adopt it.
 */
static void frame_place(FRAME *f, WIDGET *child, int x, int y, int w, int h)
{
	if (!child) return;

	/* if child is not already the content of the frame - adopt it */
	if (child != f->fd->content) frame_set_content(f, child);

	/* reposition child */
	if (x != -999999) child->gen->set_x(child, x);
	if (y != -999999) child->gen->set_y(child, y);
	if (w != -999999) child->gen->set_w(child, w);
	if (h != -999999) child->gen->set_h(child, h);
	
	f->wd->update |= WID_UPDATE_MINMAX;
	gen_methods.update(f);
}


/**
 * Request the width of the work area of a frame
 */
static int frame_get_work_w(FRAME *f)
{
	int ret = f->wd->w;
	if (f->fd->sb_y) ret -= f->fd->sb_y->scroll->get_arrow_size(f->fd->sb_y);
	return ret;
}


/**
 * Request the height of the work area of a frame
 */
static int frame_get_work_h(FRAME *f)
{
	int ret = f->wd->h;
	if (f->fd->sb_x) ret -= f->fd->sb_x->scroll->get_arrow_size(f->fd->sb_x);
	return ret;
}

/**
 * Expose specified position of frame content
 */
static void frame_expose(FRAME *f, s32 x, s32 y)
{
	WIDGET *c;
	s32 vw = get_view_w(f);
	s32 vh = get_view_h(f);

	if (!(c = f->fd->content)) return;

	/* calculate desired scroll offset */
	x -= (vw / 2);
	y -= (vh / 2);
	if (x < 0) x = 0;
	if (y < 0) y = 0;
	if (x > c->gen->get_w(c) - vw) x = c->gen->get_w(c) - vw;
	if (y > c->gen->get_h(c) - vh) y = c->gen->get_h(c) - vh;
	f->fd->scroll_x = x;
	f->fd->scroll_y = y;
	
	f->gen->updatepos(f);
	f->gen->force_redraw(f);
}


/***********************
 ** Service functions **
 ***********************/

static FRAME *create(void)
{
	FRAME *new = ALLOC_WIDGET(struct frame);
	SET_WIDGET_DEFAULTS(new, struct frame, NULL);
	new->wd->flags |= WID_FLAGS_CONCEALING;
	return new;
}


/**************************************
 ** Service structure of this module **
 **************************************/

static struct frame_services services = {
	create
};


/************************
 ** Module entry point **
 ************************/

static void build_script_lang(void)
{
	void *widtype;

	widtype = script->reg_widget_type("Frame", (void *(*)(void))create);

	script->reg_widget_attrib(widtype, "Widget content", frame_get_content, frame_set_content, gen_methods.update);
	script->reg_widget_attrib(widtype, "boolean background", frame_get_background, frame_set_background, gen_methods.update);
	script->reg_widget_attrib(widtype, "boolean scrollx", frame_get_scrollx, frame_set_scrollx, gen_methods.update);
	script->reg_widget_attrib(widtype, "boolean scrolly", frame_get_scrolly, frame_set_scrolly, gen_methods.update);
	script->reg_widget_attrib(widtype, "int workw", frame_get_work_w, NULL, NULL);
	script->reg_widget_attrib(widtype, "int workh", frame_get_work_h, NULL, NULL);
	script->reg_widget_method(widtype, "void place(Widget child,int x=-999999,int y=-999999,int w=-999999,int h=-999999)", frame_place);
	script->reg_widget_method(widtype, "void expose(int x, int y)", frame_expose);

	widman->build_script_lang(widtype, &gen_methods);
}


int init_frame(struct mtk_services *d)
{
	widman  = d->get_module("WidgetManager 1.0");
	scroll  = d->get_module("Scrollbar 1.0");
	bg      = d->get_module("Background 1.0");
	gfx     = d->get_module("Gfx 1.0");
	script  = d->get_module("Script 1.0");
	msg     = d->get_module("Messenger 1.0");

	/* define general widget functions */
	widman->default_widget_methods(&gen_methods);

	orig_updatepos = gen_methods.updatepos;
	orig_handle_event = gen_methods.handle_event;

	gen_methods.get_type     = frame_get_type;
	gen_methods.draw         = frame_draw;
	gen_methods.find         = frame_find;
	gen_methods.updatepos    = frame_updatepos;
	gen_methods.handle_event = frame_handle_event;
	gen_methods.calc_minmax  = frame_calc_minmax;
	gen_methods.free_data    = frame_free_data;
	gen_methods.remove_child = frame_remove_child;
	gen_methods.first_kfocus = frame_first_kfocus;

	build_script_lang();

	d->register_module("Frame 1.0", &services);
	return 1;
}
