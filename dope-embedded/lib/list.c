/*
 * \brief   DOpE List widget module
 * \date    2002-05-15
 * \author  Norman Feske <norman.feske@genode-labs.com>
 */

/*
 * Copyright (C) 2002-2008 Norman Feske <norman.feske@genode-labs.com>
 * Genode Labs, Feske & Helmuth Systementwicklung GbR
 *
 * This file is part of the DOpE-embedded package, which is distributed
 * under the terms of the GNU General Public License version 2.
 */

struct list;
#define WIDGET struct list

#include "dopestd.h"
#include "gfx.h"
#include "widget_data.h"
#include "widget_help.h"
#include "variable.h"
#include "list.h"
#include "fontman.h"
#include "script.h"
#include "widman.h"
#include "userstate.h"
#include "keycodes.h"
#include "messenger.h"

static struct widman_services  *widman;
static struct gfx_services     *gfx;
static struct fontman_services *font;
static struct script_services  *script;
static struct userstate_services *userstate;
static struct messenger_services *msg;

struct list_data {
	char     *text;
	s32       font_id;
	s32       ch;
	s32       sel, nsel;
	VARIABLE *var;
};

int init_list(struct dope_services *d);

#define BLACK_SOLID GFX_RGBA(0, 0, 0, 255)
#define BLACK_MIXED GFX_RGBA(0, 0, 0, 127)
#define WHITE_SOLID GFX_RGBA(255, 255, 255, 255)
#define WHITE_MIXED GFX_RGBA(255, 255, 255, 127)
#define DGRAY_MIXED GFX_RGBA(80,  80,   80, 127)
#define MGRAY_MIXED GFX_RGBA(127, 127, 127, 127)
#define LGRAY_MIXED GFX_RGBA(148, 148, 148, 127)

/********************************
 ** Functions for internal use **
 ********************************/

static void update_pos(LIST *l)
{
	char *c;
	
	l->ld->ch = font->calc_str_height(l->ld->font_id, "W");

	l->ld->nsel = 0;
	if(l->ld->text) {
		l->ld->nsel++;
		c = l->ld->text;
		while(*c != 0) {
			if(*c == '\n') l->ld->nsel++;
			c++;
		}
	}
	if(l->ld->sel >= l->ld->nsel) l->ld->sel = l->ld->nsel-1;
}

/****************************
 ** General widget methods **
 ****************************/

static inline void draw_sunken_frame(GFX_CONTAINER *d, s32 x, s32 y, s32 w, s32 h)
{
	/* outer frame */
	gfx->draw_hline(d, x + 1,     y,         w - 2, BLACK_MIXED);
	gfx->draw_vline(d, x,         y,         h,     BLACK_MIXED);
	gfx->draw_hline(d, x + 1,     y + h - 1, w - 2, BLACK_MIXED);
	gfx->draw_vline(d, x + w - 1, y,         h,     BLACK_MIXED);

	/* inner frame */
	gfx->draw_hline(d, x + 1, y + 1, w - 2, DGRAY_MIXED);
	gfx->draw_vline(d, x + 1, y + 1, h - 2, DGRAY_MIXED);
	gfx->draw_hline(d, x + 2, y + 2, w - 3, MGRAY_MIXED);
	gfx->draw_vline(d, x + 2, y + 2, h - 3, MGRAY_MIXED);
	gfx->draw_hline(d, x + 3, y + 3, w - 4, LGRAY_MIXED);
	gfx->draw_vline(d, x + 3, y + 3, h - 4, LGRAY_MIXED);
}

static inline void draw_kfocus_frame(GFX_CONTAINER *d, s32 x, s32 y, s32 w, s32 h)
{
	gfx->draw_hline(d, x + 1,     y,         w - 2, BLACK_SOLID);
	gfx->draw_vline(d, x,         y,         h,     BLACK_SOLID);
	gfx->draw_hline(d, x + 1,     y + h - 1, w - 2, BLACK_SOLID);
	gfx->draw_vline(d, x + w - 1, y,         h,     BLACK_SOLID);
}

static int lst_draw(LIST *l, struct gfx_ds *ds, long x, long y, WIDGET *origin)
{
	int tx, ty;
	int w, h;

	if (origin == l) return 1;
	if (origin) return 0;

	x += l->wd->x;
	y += l->wd->y;
	w = l->wd->w;
	h = l->wd->h;

	gfx->push_clipping(ds, x, y, w, h);

	x += 2;
	y += 2;
	w -= 2*2;
	h -= 2*2;

	if (l->wd->flags & WID_FLAGS_KFOCUS)
		draw_kfocus_frame(ds, x - 1, y - 1, w + 2, h + 2);
	
	gfx->draw_box(ds, x, y, w, h, GFX_RGB(190, 190, 190));

	draw_sunken_frame(ds, x, y, w, h);

	tx = x + 3;
	ty = y + 2;

	w -= 3;
	gfx->push_clipping(ds, x+3, y+2, w, h-3);

	if (l->ld->text) {
		gfx->draw_box(ds, tx, ty+l->ld->ch*l->ld->sel, w, l->ld->ch, GFX_RGBA(127,127,127,127));
		gfx->draw_string(ds, tx, ty, BLACK_SOLID, 0, l->ld->font_id, l->ld->text);
	}

	gfx->pop_clipping(ds);
	gfx->pop_clipping(ds);

	return 1;
}

static void (*orig_handle_event) (LIST *l, EVENT *ev, WIDGET *from);
static void lst_handle_event(LIST *l, EVENT *ev, WIDGET *from)
{
	int ypos = userstate->get_my() - l->gen->get_abs_y(l);
	int ev_done = 0;
	int s;
	char *message;

	switch (ev->type) {
	case EVENT_PRESS:
	case EVENT_KEY_REPEAT:
		switch (ev->code) {
			case DOPE_BTN_MOUSE:
				ypos -= 2 + 2;
				s = ypos/l->ld->ch;
				if(s < 0) s = 0;
				if(s >= l->ld->nsel) s = l->ld->nsel-1;
				if(l->ld->sel == s) {
					ev_done = 2;
				} else {
					l->ld->sel = s;
					ev_done = 1;
				}
				break;

			case DOPE_KEY_UP:
				if(l->ld->sel > 0)
					l->ld->sel--;
				ev_done = 1;
				break;
			case DOPE_KEY_DOWN:
				if(l->ld->sel < (l->ld->nsel-1))
					l->ld->sel++;
				ev_done = 1;
				break;

			case DOPE_KEY_HOME:
				l->ld->sel = 0;
				ev_done = 1;
				break;
			case DOPE_KEY_END: {
				l->ld->sel = l->ld->nsel-1;
				ev_done = 1;
				break;
			}

			case DOPE_KEY_TAB:
				orig_handle_event(l, ev, from);
				return;
		}

		if(ev_done == 2) {
			/* Selection committed */
			message = l->gen->get_bind_msg(l, "selcommit");
			if (message) msg->send_action_event(l->gen->get_app_id(l), "selcommit", message);
		}
		if(ev_done == 1) {
			/* Selection changed */
			message = l->gen->get_bind_msg(l, "selchange");
			if (message) msg->send_action_event(l->gen->get_app_id(l), "selchange", message);
		}
		if (ev_done)
			l->gen->force_redraw(l);
	}
}

/**
 * Determine min/max size of a list widget
 */
static void lst_calc_minmax(LIST *l)
{
	int mw, mh;
	
	l->wd->min_w = 100;
	l->wd->min_h = 100;
	if (l->ld->text) {
		mw = font->calc_str_height(l->ld->font_id, l->ld->text) + 2*2 + 3;
		mh = font->calc_str_height(l->ld->font_id, l->ld->text) + 2*2 + 3;
		if (l->wd->min_w < mw) l->wd->min_w = mw;
		if (l->wd->min_h < mh) l->wd->min_h = mh;
	}
	l->wd->max_w = 99999;
	l->wd->max_h = 99999;
}


static void (*orig_updatepos) (LIST *l);
static void lst_updatepos(LIST *l)
{
	orig_updatepos(l);
	update_pos(l);
}


/**
 * Free list widget data
 */
static void lst_free_data(LIST *l)
{
	if (l->ld->text) free(l->ld->text);
}


/**
 * Return widget type identifier
 */
static char *lst_get_type(LIST *l)
{
	return "List";
}


/****************************
 ** List specific methods **
 ****************************/

static void lst_set_text(LIST *l, char *new_txt)
{
	if ((!l) || (!l->ld)) return;
	if (l->ld->text) free(l->ld->text);
	l->ld->text = dope_strdup(new_txt);
	l->wd->update |= WID_UPDATE_MINMAX;
}


static char *lst_get_text(LIST *l)
{
	return l->ld->text;
}


/**
 * Set font of list
 *
 * Currently, only the font identifiers 'default',
 * 'monospaced' and 'title' are defined.
 */
static void lst_set_font(LIST *l, char *fontname)
{
	if (dope_streq(fontname, "default", 255)) {
		l->ld->font_id = 0;
	} else if (dope_streq(fontname, "monospaced", 255)) {
		l->ld->font_id = 1;
	} else if (dope_streq(fontname, "title", 255)) {
		l->ld->font_id = 2;
	}
	l->wd->update |= WID_UPDATE_MINMAX;
}

/**
 * Request currently used font of a list
 */
static char *lst_get_font(LIST *l)
{
	switch (l->ld->font_id) {
		case 1:  return "monospaced";
		case 2:  return "title";
		default: return "default";
	}
}

void lst_set_selection(LIST *l, int selection)
{
	l->ld->sel = selection;
	if(l->ld->sel < 0) l->ld->sel = 0;
	if(l->ld->sel >= l->ld->nsel) l->ld->sel = l->ld->nsel-1;
	l->gen->force_redraw(l);
}

int lst_get_selection(LIST *l)
{
	return l->ld->sel;
}

static struct widget_methods gen_methods;
static struct list_methods lst_methods = {
	lst_set_text,
	lst_get_text,
	lst_set_font,
	lst_get_font,
	lst_set_selection,
	lst_get_selection,
};


/***********************
 ** Service functions **
 ***********************/

static LIST *create(void)
{
	LIST *new = ALLOC_WIDGET(struct list);
	SET_WIDGET_DEFAULTS(new, struct list, &lst_methods);

	/* set list specific attributes */
	new->ld->text = dope_strdup("");
	new->ld->nsel = 0;
	new->wd->flags |= WID_FLAGS_EDITABLE | WID_FLAGS_TAKEFOCUS;
	update_pos(new);

	return new;
}


/**************************************
 ** Service structure of this module **
 **************************************/

static struct list_services services = {
	create
};


/************************
 ** Module entry point **
 ************************/

static void build_script_lang(void)
{
	void *widtype;

	widtype = script->reg_widget_type("List", (void *(*)(void))create);

	script->reg_widget_attrib(widtype, "string text", lst_get_text, lst_set_text, gen_methods.update);
	script->reg_widget_attrib(widtype, "string font", lst_get_font, lst_set_font, gen_methods.update);
	script->reg_widget_attrib(widtype, "integer selection", lst_get_selection, lst_set_selection, gen_methods.update);

	widman->build_script_lang(widtype, &gen_methods);
}


int init_list(struct dope_services *d)
{
	widman    = d->get_module("WidgetManager 1.0");
	gfx       = d->get_module("Gfx 1.0");
	font      = d->get_module("FontManager 1.0");
	script    = d->get_module("Script 1.0");
	userstate = d->get_module("UserState 1.0");
	msg       = d->get_module("Messenger 1.0");

	/* define general widget functions */
	widman->default_widget_methods(&gen_methods);

	orig_updatepos = gen_methods.updatepos;
	orig_handle_event = gen_methods.handle_event;

	gen_methods.draw         = lst_draw;
	gen_methods.updatepos    = lst_updatepos;
	gen_methods.handle_event = lst_handle_event;
	gen_methods.get_type     = lst_get_type;
	gen_methods.calc_minmax  = lst_calc_minmax;
	gen_methods.free_data    = lst_free_data;

	build_script_lang();

	d->register_module("List 1.0", &services);
	return 1;
}
