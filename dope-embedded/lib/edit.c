/*
 * \brief   DOpE Edit widget module
 * \date    2004-04-07
 * \author  Norman Feske <norman.feske@genode-labs.com>
 */

/*
 * Copyright (C) 2004-2008 Norman Feske <norman.feske@genode-labs.com>
 * Genode Labs, Feske & Helmuth Systementwicklung GbR
 *
 * This file is part of the DOpE-embedded package, which is distributed
 * under the terms of the GNU General Public License version 2.
 */

struct edit;
#define WIDGET struct edit

#include "dopestd.h"
#include "edit.h"
#include "gfx_macros.h"
#include "widget_data.h"
#include "widget_help.h"
#include "fontman.h"
#include "script.h"
#include "widman.h"
#include "userstate.h"
#include "messenger.h"
#include "keycodes.h"
#include "tick.h"

static struct widman_services    *widman;
static struct gfx_services       *gfx;
static struct fontman_services   *font;
static struct script_services    *script;
static struct userstate_services *userstate;
static struct messenger_services *msg;
static struct tick_services      *tick;

struct edit_data {
	s16    font_id;                  /* used font                      */
	s16    flags;                    /* edit properties               */
	s16    tx;                       /* adaptive text position         */
	s16    ty;                       /* text position inside the edit */
	s32    tw, th;                   /* pixel width and height of text */
	s32    sel_beg, sel_end;         /* current selection              */
	s32    sel_x, sel_w;             /* pixel position of selection    */
	s32    curpos;                   /* position of cursor             */
	s32    cx, ch;                   /* cursor x position and height   */
	s16    pad_x, pad_y;             /* padding aroung edit           */
	char  *txtbuf;                   /* textual content of the edit   */
	s32    txtbuflen;                /* current size of text buffer    */
	s32    maxlen;                   /* max string length              */
	s32    vislen;                   /* min visible length             */
	void (*click)  (WIDGET *);
	void (*release)(WIDGET *);
};

int init_edit(struct dope_services *d);

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

/**
 * Delete character at specified string position
 *
 * \return   1 on success
 */
static int delete_char(EDIT *e, int idx)
{
	char *dst = e->ed->txtbuf + idx;
	char *src = e->ed->txtbuf + idx + 1;
	if (idx >= strlen(e->ed->txtbuf)) return 0;
	while (*src) *(dst++) = *(src++);
	*dst = 0;
	return 1;
}


/**
 * Insert character at specified string position
 *
 * If the string buffer exceeds, a new one is allocated.
 *
 * \return   1 on success
 */
static int insert_char(EDIT *e, int idx, char c)
{
	int i, len = strlen(e->ed->txtbuf);
	char *src, *dst;

	/* if txtbuffer exceeds, reallocate a bigger one */
	if (len >= e->ed->txtbuflen) {
		char *new;
		e->ed->txtbuflen = e->ed->txtbuflen * 2 + 1;
		new = (char *)zalloc(e->ed->txtbuflen);
		if (!new) return 0;
		for(i=0; i<=len; i++) new[i] = e->ed->txtbuf[i];
		free(e->ed->txtbuf);
		e->ed->txtbuf = new;
	}
	dst = e->ed->txtbuf + len;
	src = dst - 1;

	/* move tail of string to the right */
	for (i=0; i<len-idx; i++) *(dst--) = *(src--);

	/* insert character */
	*dst = c;
	return 1;
}

/**
 * Calculate x position of the character at the specified index
 */
static inline int get_char_pos(EDIT *e, int idx)
{
	int xpos, tmp = e->ed->txtbuf[idx];

	e->ed->txtbuf[idx] = 0;
	xpos = font->calc_str_width(e->ed->font_id, e->ed->txtbuf);
	e->ed->txtbuf[idx] = tmp;
	return xpos;
}


/**
 * Calculate character index that corresponds to the specified position
 */
static inline int get_char_index(EDIT *e, int pos)
{
	return font->calc_char_idx(e->ed->font_id, e->ed->txtbuf, pos);
}


/**
 * Calculate text and cursor pixel positions
 */
static void update_text_pos(EDIT *e)
{
	int vw;
	if (!e || !e->ed || !e->ed->txtbuf) return;

	e->ed->ty = 0;
	e->ed->tw = font->calc_str_width (e->ed->font_id, e->ed->txtbuf);
	e->ed->th = font->calc_str_height(e->ed->font_id, e->ed->txtbuf);
	e->ed->ch = e->ed->th;

	/* calculate x position of cursor */
	e->ed->cx = get_char_pos(e, e->ed->curpos);

	if (e->ed->sel_beg < e->ed->sel_end) {
		e->ed->sel_x = get_char_pos(e, e->ed->sel_beg);
		e->ed->sel_w = get_char_pos(e, e->ed->sel_end) - e->ed->sel_x + 1;
	} else if (e->ed->sel_beg > e->ed->sel_end) {
		e->ed->sel_x = get_char_pos(e, e->ed->sel_end);
		e->ed->sel_w = get_char_pos(e, e->ed->sel_beg) - e->ed->sel_x + 1;
	} else {
		e->ed->sel_x = e->ed->sel_w = 0;
	}

	/* set text position so that the cursor is visible */
	e->ed->tx = 0;
	vw = e->wd->w - 2*e->ed->pad_x - 6;   /* visible width */

	if ((e->ed->tx > vw - e->ed->tw) && (e->ed->tx > vw + 2 - (vw/3) - e->ed->cx)) {
		e->ed->tx = vw - (vw/3) - e->ed->cx;
		if (e->ed->tx < vw - e->ed->tw)
			e->ed->tx = vw - e->ed->tw;
	}

	if (e->ed->tx <    - e->ed->cx) e->ed->tx =    - e->ed->cx;
	if (e->ed->tx > vw - e->ed->cx) e->ed->tx = vw - e->ed->cx;
}

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


/****************************
 ** General widget methods **
 ****************************/

static int edit_draw(EDIT *e, struct gfx_ds *ds, long x, long y, WIDGET *origin)
{
	long tx = e->ed->tx, ty = e->ed->ty;
	u32  tc = GFX_RGB(32, 32, 32);
	u32  cc = BLACK_MIXED;
	s32  cx;
	long w  = e->wd->w,  h  = e->wd->h;

	if (origin == e) return 1;
	if (origin) return 0;

	x += e->wd->x;
	y += e->wd->y;

	gfx->push_clipping(ds, x, y, w, h);

	x += e->ed->pad_x;
	y += e->ed->pad_y;
	w -= e->ed->pad_x*2;
	h -= e->ed->pad_y*2;

	if (e->wd->flags & WID_FLAGS_KFOCUS)
		draw_kfocus_frame(ds, x - 1, y - 1, w + 2, h + 2);

	if (e->wd->flags & WID_FLAGS_MFOCUS)
		gfx->draw_box(ds, x, y, w, h, GFX_RGB(190, 190, 190));
	else
		gfx->draw_box(ds, x, y, w, h, GFX_RGB(162, 162, 162));

	draw_sunken_frame(ds, x, y, w, h);

	if (e->wd->flags & WID_FLAGS_MFOCUS) tc = cc = BLACK_SOLID;

	tx += x + 3; ty += y + 2;

	gfx->push_clipping(ds, x+2, y+2, w-3, h-3);

	/* draw selection */
	if (e->ed->sel_w)
		gfx->draw_box(ds, e->ed->sel_x + tx - 1, ty, e->ed->sel_w, e->ed->ch+1, GFX_RGBA(127,127,127,127));

	if (e->ed->txtbuf)
		gfx->draw_string(ds, tx, ty, tc, 0, e->ed->font_id, e->ed->txtbuf);

	/* draw cursor */
	if (e->wd->flags & WID_FLAGS_KFOCUS) {
		cx = tx + e->ed->cx - 2;
		gfx->draw_vline(ds, cx + 1, ty, e->ed->ch, cc);
		gfx->draw_hline(ds, cx, ty, 3, cc);
		gfx->draw_hline(ds, cx, ty + e->ed->ch - 1, 3, cc);
	}

	gfx->pop_clipping(ds);
	gfx->pop_clipping(ds);
	return 1;
}


static EDIT *edit_find(EDIT *e, long x, long y)
{
	x -= e->wd->x;
	y -= e->wd->y;

	/* check if position is inside the edit */
	if ((x >= e->ed->pad_x) && (x < e->wd->w - e->ed->pad_x)
	 && (y >= e->ed->pad_y) && (y < e->wd->h - e->ed->pad_y))
		return e;

	return NULL;
}


static void sel_tick(EDIT *e, int dx, int dy)
{
	int mx = userstate->get_mx();
	int lx = e->gen->get_abs_x(e);
	int xpos = mx - lx - e->ed->tx - e->ed->pad_x - 3;
	int csel;
	int vw = e->wd->w - 2*e->ed->pad_x - 6;

	/* mouse beyond left widget border - scroll text area */
	if (mx < lx) {
		e->ed->tx += (lx - mx)/4;
		if (e->ed->tx > 0) e->ed->tx = 0;
		xpos = -e->ed->tx;
	}
	/* mouse beyond right widget border - scroll text area */
	if (mx > lx + e->wd->w) {
		e->ed->tx += (lx + e->wd->w - mx)/4;
		if (e->ed->tx + e->ed->tw < vw) e->ed->tx = vw - e->ed->tw;
		if (e->ed->tx > 0) e->ed->tx = 0;
		xpos = vw - e->ed->tx;
	}

	csel = get_char_index(e, xpos);
	e->ed->sel_end = csel;

	update_text_pos(e);
	e->gen->force_redraw(e);
}


static void sel_release(EDIT *e, int dx, int dy)
{
	e->ed->curpos = e->ed->sel_end;
	update_text_pos(e);
}


static void (*orig_handle_event) (EDIT *e, EVENT *ev, WIDGET *from);
static void edit_handle_event(EDIT *e, EVENT *ev, WIDGET *from)
{
	int xpos = userstate->get_mx() - e->gen->get_abs_x(e);
	int ascii;
	int ev_done = 0;

	switch (ev->type) {
	case EVENT_PRESS:
	case EVENT_KEY_REPEAT:
		e->ed->sel_beg = e->ed->sel_end = e->ed->sel_w = 0;
		switch (ev->code) {
			case DOPE_BTN_MOUSE:
				xpos -= e->ed->tx + e->ed->pad_x + 3;
				e->ed->curpos = get_char_index(e, xpos);
				e->ed->sel_beg = e->ed->sel_end = e->ed->curpos;
				e->ed->sel_w = 0;
				userstate->drag(e, NULL, sel_tick, sel_release);
				ev_done = 1;
				break;
			case DOPE_KEY_LEFT:
				if (e->ed->curpos > 0) e->ed->curpos--;
				ev_done = 2;
				break;

			case DOPE_KEY_RIGHT:
				if (e->ed->curpos < strlen(e->ed->txtbuf)) e->ed->curpos++;
				ev_done = 2;
				break;

			case DOPE_KEY_HOME:
				e->ed->curpos = 0;
				ev_done = 2;
				break;

			case DOPE_KEY_END:
				e->ed->curpos = strlen(e->ed->txtbuf);
				ev_done = 2;
				break;

			case DOPE_KEY_DELETE:
				if (e->ed->curpos < strlen(e->ed->txtbuf))
					delete_char(e, e->ed->curpos);
				ev_done = 2;
				break;

			case DOPE_KEY_BACKSPACE:
				if (e->ed->curpos > 0) {
					e->ed->curpos--;
					delete_char(e, e->ed->curpos);
				}
				ev_done = 2;
				break;

			case DOPE_KEY_TAB:
				orig_handle_event(e, ev, from);
				return;
		}

		if (ev_done) {
			update_text_pos(e);
			e->gen->force_redraw(e);
			return;
		}

		/* insert ASCII character */
		ascii = userstate->get_ascii(ev->code);
		if (!ascii) return;
		insert_char(e, e->ed->curpos, ascii);
		e->ed->curpos++;
		e->wd->update |= WID_UPDATE_REFRESH;
		e->gen->update(e);
	}
}


/**
 * Determine min/max size of a edit
 *
 * The height and the min width of a Edit depend on the used font and
 * the Edit text.
 */
static void edit_calc_minmax(EDIT *e)
{
	e->wd->min_w = e->ed->pad_x*2 + 4 + e->ed->vislen*font->calc_str_width(e->ed->font_id, "W");
	e->wd->max_w = 99999;
	e->wd->min_h = font->calc_str_height(e->ed->font_id, "W")
	                            + e->ed->pad_y*2 + 4;
	e->wd->max_h = 99999;
}


/**
 * Update position and size of edit
 *
 * When size changes, the text position must be updated.
 */
static void (*orig_updatepos) (EDIT *e);
static void edit_updatepos(EDIT *e)
{
	update_text_pos(e);
	orig_updatepos(e);
}

/**
 * Free edit widget data
 */
static void edit_free_data(EDIT *e)
{
	if (e->ed->txtbuf) free(e->ed->txtbuf);
}


/**
 * Return widget type identifier
 */
static char *edit_get_type(EDIT *e)
{
	return "Edit";
}


/****************************
 ** Edit specific methods **
 ****************************/

static void edit_set_text(EDIT *e, char *new_txt)
{
	int i;

	if (!new_txt) return;

	/* clear old text */
	while (e->ed->txtbuf[0]) delete_char(e, 0);

	/* insert new charaters */
	for (i = 0; new_txt[i]; i++) insert_char(e, i, new_txt[i]);

	/* make the new text visible */
	e->wd->update |= WID_UPDATE_REFRESH;
}


static char *edit_get_text(EDIT *e)
{
	if (!e || !e->ed) return 0;
	return e->ed->txtbuf;
}

static struct widget_methods gen_methods;
static struct edit_methods edit_methods = {
	edit_set_text,
	edit_get_text,
};


/***********************
 ** Service functions **
 ***********************/

static EDIT *create(void)
{
	EDIT *new = ALLOC_WIDGET(struct edit);
	SET_WIDGET_DEFAULTS(new, struct edit, &edit_methods);

	/* set edit specific attributes */
	new->ed->pad_x     = new->ed->pad_y = 2;
	new->ed->vislen    = 2;
	new->ed->sel_beg   = -1;
	new->ed->sel_end   = -1;
	new->ed->txtbuflen = 16;
	new->ed->txtbuf    = zalloc(new->ed->txtbuflen);
	new->wd->flags    |= WID_FLAGS_EDITABLE | WID_FLAGS_HIGHLIGHT;

	/* let the edit receive the keyboard focus even without any bindings */
	new->wd->flags    |= WID_FLAGS_TAKEFOCUS;

	update_text_pos(new);
	edit_calc_minmax(new);
	return new;
}


/**************************************
 ** Service structure of this module **
 **************************************/

static struct edit_services services = {
	create
};


/************************
 ** Module edit point **
 ************************/

static void build_script_lang(void)
{
	void *widtype;

	widtype = script->reg_widget_type("Edit", (void *(*)(void))create);
	script->reg_widget_attrib(widtype, "string text", edit_get_text, edit_set_text, gen_methods.update);
	widman->build_script_lang(widtype, &gen_methods);
}


int init_edit(struct dope_services *d)
{
	widman    = d->get_module("WidgetManager 1.0");
	gfx       = d->get_module("Gfx 1.0");
	font      = d->get_module("FontManager 1.0");
	script    = d->get_module("Script 1.0");
	userstate = d->get_module("UserState 1.0");
	msg       = d->get_module("Messenger 1.0");
	tick      = d->get_module("Tick 1.0");

	/* define general widget functions */
	widman->default_widget_methods(&gen_methods);

	orig_updatepos    = gen_methods.updatepos;
	orig_handle_event = gen_methods.handle_event;

	gen_methods.get_type     = edit_get_type;
	gen_methods.draw         = edit_draw;
	gen_methods.updatepos    = edit_updatepos;
	gen_methods.handle_event = edit_handle_event;
	gen_methods.calc_minmax  = edit_calc_minmax;
	gen_methods.find         = edit_find;
	gen_methods.free_data    = edit_free_data;

	build_script_lang();

	d->register_module("Edit 1.0", &services);
	return 1;
}
