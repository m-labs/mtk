/*
 * \brief   MTK Entry widget module
 */

/*
 * Copyright (C) 2004-2008 Norman Feske <norman.feske@genode-labs.com>
 * Genode Labs, Feske & Helmuth Systementwicklung GbR
 * Copyright (C) 2010 Romain P <rom1@netcourrier.com>
 *
This file is part of the MTK package, which is distributed
 * under the terms of the GNU General Public License version 2.
 */

struct entry;
#define WIDGET struct entry

#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "mtkstd.h"
#include "entry.h"
#include "gfx_macros.h"
#include "widget_data.h"
#include "widget_help.h"
#include "fontman.h"
#include "script.h"
#include "widman.h"
#include "userstate.h"
#include "messenger.h"
#include "keycodes.h"
#include "clipboard.h"

static struct widman_services    *widman;
static struct gfx_services       *gfx;
static struct fontman_services   *font;
static struct script_services    *script;
static struct userstate_services *userstate;
static struct messenger_services *msg;
static struct clipboard_services *clipb;

struct entry_data {
	s16    font_id;                  /* used font                      */
	s16    flags;                    /* entry properties               */
	s32    tx;                       /* text position         */
	s32    ty;                       /* text position inside the entry */
	s32    tw, th;                   /* pixel width and height of text */
	s32    sel_beg, sel_end;         /* current selection              */
	s32    sel_x, sel_w;             /* pixel position of selection    */
	s32    curpos;                   /* position of cursor             */
	s32    cx, ch;                   /* cursor x position and height   */
	char  *txtbuf;                   /* textual content of the entry   */
	s32    txtbuflen;                /* current size of text buffer    */
	s32    maxlen;                   /* max string length              */
	s32    vislen;                   /* min visible length             */
	void (*click)  (WIDGET *);
	void (*release)(WIDGET *);
};

#define ENTRY_FLAGS_BLIND    0x1   /* blind out characters */
#define ENTRY_FLAGS_READONLY 0x2

static GFX_CONTAINER *normal_img;
static GFX_CONTAINER *focus_img;
static GFX_CONTAINER *readonly_img;

int init_entry(struct mtk_services *d);

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

static void notify_change(ENTRY *e)
{
	int app_id;
	char *m;

	m = e->gen->get_bind_msg(e, "change");
	app_id = e->gen->get_app_id(e);
	if (m) msg->send_action_event(app_id, "change", m);
}

/**
 * Delete character at specified string position
 *
 * \return   1 on success
 */
static int delete_char(ENTRY *e, int idx)
{
	char *dst = e->ed->txtbuf + idx;
	char *src = e->ed->txtbuf + idx + 1;
	if (idx >= strlen(e->ed->txtbuf)) return 0;
	while (*src) *(dst++) = *(src++);
	*dst = 0;
	notify_change(e);
	return 1;
}


/**
 * Insert character at specified string position
 *
 * If the string buffer exceeds, a new one is allocated.
 *
 * \return   1 on success
 */
static int insert_char(ENTRY *e, int idx, char c)
{
	int i, len = strlen(e->ed->txtbuf);
	char *src, *dst;

	/* if txtbuffer exceeds, reallocate a bigger one */
	if (len >= (e->ed->txtbuflen-1)) {
		char *new;
		e->ed->txtbuflen = e->ed->txtbuflen * 2;
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

	notify_change(e);
	return 1;
}

/**
 * Calculate x position of the character at the specified index
 */
static inline int get_char_pos(ENTRY *e, int idx)
{
	int xpos, tmp = e->ed->txtbuf[idx];

	if (e->ed->flags & ENTRY_FLAGS_BLIND)
		return idx*font->calc_str_width(e->ed->font_id, "*");

	e->ed->txtbuf[idx] = 0;
	xpos = font->calc_str_width(e->ed->font_id, e->ed->txtbuf);
	e->ed->txtbuf[idx] = tmp;
	return xpos;
}


/**
 * Calculate character index that corresponds to the specified position
 */
static inline int get_char_index(ENTRY *e, s32 pos)
{
	if (e->ed->flags & ENTRY_FLAGS_BLIND) {
		int step = font->calc_str_width(e->ed->font_id, "*");
		int res = step ? pos/step : 0;
		if (res > strlen(e->ed->txtbuf)) res = strlen(e->ed->txtbuf);
		return res;
	}
	return font->calc_char_idx(e->ed->font_id, e->ed->txtbuf, pos, 0);
}


/**
 * Calculate text and cursor pixel positions
 */
static void update_text_pos(ENTRY *e)
{
	int vw;
	int ref;
	int delta;

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

	/* set text position so that the cursor or the end of selection (if any) is visible */
	if(e->ed->sel_w > 0) {
		if(e->ed->sel_beg < e->ed->sel_end)
			ref = e->ed->sel_x + e->ed->sel_w - 1;
		else
			ref = e->ed->sel_x;
	} else
		ref = e->ed->cx;
	vw = e->wd->w - 2*2 - 6;   /* visible width */
	delta = e->ed->tx + ref;
	if(delta > vw)
		e->ed->tx = vw - ref;
	if(delta < 0)
		e->ed->tx = -ref;
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

static int entry_draw(ENTRY *e, struct gfx_ds *ds, int x, int y, WIDGET *origin)
{
	int tx = e->ed->tx, ty = e->ed->ty;
	u32  tc = GFX_RGB(32, 32, 32);
	u32  cc = BLACK_MIXED;
	s32  cx;
	int w  = e->wd->w,  h  = e->wd->h;

	if (origin == e) return 1;
	if (origin) return 0;

	x += e->wd->x;
	y += e->wd->y;

	gfx->push_clipping(ds, x, y, w, h);

	x += 2;
	y += 2;
	w -= 2*2;
	h -= 2*2;

	if (e->wd->flags & WID_FLAGS_KFOCUS)
		draw_kfocus_frame(ds, x - 1, y - 1, w + 2, h + 2);

	if (e->ed->flags & ENTRY_FLAGS_READONLY)
		gfx->draw_img(ds, x, y, w, h, readonly_img, 255);
	else {
		if (e->wd->flags & WID_FLAGS_MFOCUS)
			gfx->draw_img(ds, x, y, w, h, focus_img, 255);
		else
			gfx->draw_img(ds, x, y, w, h, normal_img, 255);
	}

	draw_sunken_frame(ds, x, y, w, h);

	if (e->wd->flags & WID_FLAGS_MFOCUS) tc = cc = BLACK_SOLID;

	tx += x + 3; ty += y + 2;

	gfx->push_clipping(ds, x+2, y+2, w-3, h-3);

	/* draw selection */
	if (e->ed->sel_w)
		gfx->draw_box(ds, e->ed->sel_x + tx - 1, ty, e->ed->sel_w, e->ed->ch+1, GFX_RGBA(127,127,127,127));

	if (e->ed->txtbuf) {
		if (e->ed->flags & ENTRY_FLAGS_BLIND) {
			int step = font->calc_str_width(e->ed->font_id, "*");
			int i, len = strlen(e->ed->txtbuf);
			for (i = 0; i < len; i++) {
				gfx->draw_string(ds, tx + step*i, ty, tc, 0, e->ed->font_id, "*");
			}
		} else gfx->draw_string(ds, tx, ty, tc, 0, e->ed->font_id, e->ed->txtbuf);
	}

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


static ENTRY *entry_find(ENTRY *e, int x, int y)
{
	x -= e->wd->x;
	y -= e->wd->y;

	/* check if position is inside the entry */
	if ((x >= 2) && (x < e->wd->w - 2)
	 && (y >= 2) && (y < e->wd->h - 2))
		return e;

	return NULL;
}


static void sel_tick(ENTRY *e, int dx, int dy)
{
	int mx = userstate->get_mx();
	int lx = e->gen->get_abs_x(e);
	int xpos = mx - lx - e->ed->tx - 2 - 3;
	int csel;
	int vw = e->wd->w - 2*2 - 6;

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


static void sel_release(ENTRY *e, int dx, int dy)
{
	e->ed->curpos = e->ed->sel_end;
	update_text_pos(e);
}

static void sel_reset(ENTRY *e)
{
	e->ed->sel_beg = e->ed->sel_end = e->ed->curpos;
}

static void sel_copy(ENTRY *e)
{
	s32 start_idx = MIN(e->ed->sel_beg, e->ed->sel_end);
	s32 end_idx = MAX(e->ed->sel_beg, e->ed->sel_end);
	if(start_idx == end_idx) return;
	clipb->set(e->ed->txtbuf + start_idx, end_idx - start_idx);
	e->ed->curpos = e->ed->sel_beg;
}

static void sel_cut(ENTRY *e)
{
	s32 i;
	s32 start_idx = MIN(e->ed->sel_beg, e->ed->sel_end);
	s32 end_idx = MAX(e->ed->sel_beg, e->ed->sel_end);
	if(start_idx == end_idx) return;
	clipb->set(e->ed->txtbuf + start_idx, end_idx - start_idx);
	for(i = start_idx; i<end_idx; ++i){
		delete_char(e, start_idx);
	}
	e->ed->curpos = start_idx;
}

static void clipboard_paste(ENTRY *e)
{
	char* clip_data;
	s32 clip_length;
	int i, j=0;

	clipb->get(&clip_data, &clip_length);
	if(clip_length == 0) return;
	for(i = 0; i<clip_length; ++i){
		if(clip_data[i] != '\n'){
			insert_char(e, e->ed->curpos + j, clip_data[i]);
			++j;
		}
	}
	e->ed->curpos += j;
}

static void (*orig_handle_event) (ENTRY *e, EVENT *ev, WIDGET *from);
static void entry_handle_event(ENTRY *e, EVENT *ev, WIDGET *from)
{
	int xpos = userstate->get_mx() - e->gen->get_abs_x(e);
	int ascii;
	int ev_done = 0;
	static char ctrl_pressed = 0;
	s32 old_tx;

	switch (ev->type) {
	case EVENT_RELEASE:
		if(ev->code == MTK_KEY_LEFTCTRL)
			ctrl_pressed = 0;
		break;
	case EVENT_PRESS:
	case EVENT_KEY_REPEAT:
		switch(ev->code) {
			case MTK_BTN_MOUSE:
				xpos -= e->ed->tx + 2 + 3;
				e->ed->curpos = get_char_index(e, xpos);
				e->ed->sel_beg = e->ed->sel_end = e->ed->curpos;
				e->ed->sel_w = 0;
				old_tx = e->ed->tx;
				update_text_pos(e);
				if(old_tx == e->ed->tx)
					userstate->drag(e, NULL, sel_tick, sel_release);
				e->gen->force_redraw(e);
				return;
			case MTK_KEY_LEFT:
				if (e->ed->curpos > 0) e->ed->curpos--;
				sel_reset(e);
				ev_done = 2;
				break;

			case MTK_KEY_RIGHT:
				if (e->ed->curpos < strlen(e->ed->txtbuf)) e->ed->curpos++;
				sel_reset(e);
				ev_done = 2;
				break;

			case MTK_KEY_HOME:
				e->ed->curpos = 0;
				sel_reset(e);
				ev_done = 2;
				break;

			case MTK_KEY_END:
				e->ed->curpos = strlen(e->ed->txtbuf);
				sel_reset(e);
				ev_done = 2;
				break;

			case MTK_KEY_DELETE:
				if (!(e->ed->flags & ENTRY_FLAGS_READONLY)) {
					if (e->ed->curpos < strlen(e->ed->txtbuf))
						delete_char(e, e->ed->curpos);
					sel_reset(e);
					ev_done = 2;
				}
				break;

			case MTK_KEY_BACKSPACE:
				if (!(e->ed->flags & ENTRY_FLAGS_READONLY)) {
					if (e->ed->curpos > 0) {
						e->ed->curpos--;
						delete_char(e, e->ed->curpos);
					}
					sel_reset(e);
					ev_done = 2;
				}
				break;

			case MTK_KEY_LEFTCTRL:
				ctrl_pressed = 1;
				break;

			case MTK_KEY_C:
				if(ctrl_pressed) {
					sel_copy(e);
					ev_done = 2;
				}
				break;

			case MTK_KEY_X:
				if(ctrl_pressed && !(e->ed->flags & ENTRY_FLAGS_READONLY)) {
					sel_cut(e);
					sel_reset(e);
					ev_done = 2;
				}
				break;

			case MTK_KEY_V:
				if(ctrl_pressed && !(e->ed->flags & ENTRY_FLAGS_READONLY)) {
					clipboard_paste(e);
					sel_reset(e);
					ev_done = 2;
				}
				break;

			case MTK_KEY_ENTER: {
					/* send commit event to client application */
					char *m = e->gen->get_bind_msg(e, "commit");
					int app_id = e->gen->get_app_id(e);
					if (m) msg->send_action_event(app_id, "commit", m);
				}
				ev_done = 1;
				break;

			case MTK_KEY_TAB:
				orig_handle_event(e, ev, from);
				return;
		}

		if (ev_done) {
			update_text_pos(e);
			e->gen->force_redraw(e);
			return;
		}

		ascii = userstate->get_ascii(ev->code);
		if (!ascii) {
			orig_handle_event(e, ev, from);
			return;
		}
		/* insert ASCII character */
		if(!(e->ed->flags & ENTRY_FLAGS_READONLY)) {
			insert_char(e, e->ed->curpos, ascii);
			sel_reset(e);
			e->ed->curpos++;
			e->wd->update |= WID_UPDATE_REFRESH;
			e->gen->update(e);
		}
	}
}


/**
 * Determine min/max size of a entry
 *
 * The height and the min width of a Entry depend on the used font and
 * the Entry text.
 */
static void entry_calc_minmax(ENTRY *e)
{
	e->wd->min_w = 2*2 + 4 + e->ed->vislen*font->calc_str_width(e->ed->font_id, "W");
	e->wd->max_w = 99999;
	e->wd->min_h = e->wd->max_h = font->calc_str_height(e->ed->font_id, "W")
	                            + 2*2 + 4;
}


/**
 * Update position and size of entry
 *
 * When size changes, the text position must be updated.
 */
static void (*orig_updatepos) (ENTRY *e);
static void entry_updatepos(ENTRY *e)
{
	update_text_pos(e);
	orig_updatepos(e);
}

/**
 * Free entry widget data
 */
static void entry_free_data(ENTRY *e)
{
	if (e->ed->txtbuf) free(e->ed->txtbuf);
}


/**
 * Return widget type identifier
 */
static char *entry_get_type(ENTRY *e)
{
	return "Entry";
}


/****************************
 ** Entry specific methods **
 ****************************/

static void entry_set_text(ENTRY *e, char *new_txt)
{
	char *newt;

	if(!new_txt) return;
	newt = strdup(new_txt);
	if(!newt) return;

	free(e->ed->txtbuf);
	e->ed->txtbuf = newt;
	e->ed->txtbuflen = strlen(new_txt) + 1;

	/* make the new text visible */
	e->wd->update |= WID_UPDATE_REFRESH;
}


static char *entry_get_text(ENTRY *e)
{
	if (!e || !e->ed) return 0;
	return e->ed->txtbuf;
}

static void entry_set_blind(ENTRY *e, int blind_flag)
{
	if (blind_flag)
		e->ed->flags |= ENTRY_FLAGS_BLIND;
	else
		e->ed->flags &= ~ENTRY_FLAGS_BLIND;

	e->wd->update |= WID_UPDATE_REFRESH;
}

static int entry_get_blind(ENTRY *e)
{
	return !!(e->ed->flags & ENTRY_FLAGS_BLIND);
}

static void entry_set_readonly(ENTRY *e, int readonly_flag)
{
	if (readonly_flag)
		e->ed->flags |= ENTRY_FLAGS_READONLY;
	else
		e->ed->flags &= ~ENTRY_FLAGS_READONLY;

	e->wd->update |= WID_UPDATE_REFRESH;
}

static int entry_get_readonly(ENTRY *e)
{
	return !!(e->ed->flags & ENTRY_FLAGS_READONLY);
}

static struct widget_methods gen_methods;
static struct entry_methods entry_methods = {
	entry_set_text,
	entry_get_text,
};


/***********************
 ** Service functions **
 ***********************/

static ENTRY *create(void)
{
	ENTRY *new = ALLOC_WIDGET(struct entry);
	SET_WIDGET_DEFAULTS(new, struct entry, &entry_methods);

	/* set entry specific attributes */
	new->ed->vislen    = 2;
	new->ed->sel_beg   = -1;
	new->ed->sel_end   = -1;
	new->ed->txtbuflen = 16;
	new->ed->txtbuf    = zalloc(new->ed->txtbuflen);
	new->wd->flags    |= WID_FLAGS_EDITABLE | WID_FLAGS_HIGHLIGHT;

	/* let the entry receive the keyboard focus even without any bindings */
	new->wd->flags    |= WID_FLAGS_TAKEFOCUS;

	update_text_pos(new);
	entry_calc_minmax(new);
	return new;
}


/**************************************
 ** Service structure of this module **
 **************************************/

static struct entry_services services = {
	create
};


/************************
 ** Module entry point **
 ************************/

static void build_script_lang(void)
{
	void *widtype;

	widtype = script->reg_widget_type("Entry", (void *(*)(void))create);
	script->reg_widget_attrib(widtype, "string text", entry_get_text, entry_set_text, gen_methods.update);
	script->reg_widget_attrib(widtype, "boolean blind", entry_get_blind, entry_set_blind, gen_methods.update);
	script->reg_widget_attrib(widtype, "boolean readonly", entry_get_readonly, entry_set_readonly, gen_methods.update);
	widman->build_script_lang(widtype, &gen_methods);
}


int init_entry(struct mtk_services *d)
{
	widman    = d->get_module("WidgetManager 1.0");
	gfx       = d->get_module("Gfx 1.0");
	font      = d->get_module("FontManager 1.0");
	script    = d->get_module("Script 1.0");
	userstate = d->get_module("UserState 1.0");
	msg       = d->get_module("Messenger 1.0");
	clipb     = d->get_module("Clipboard 1.0");

	normal_img = gen_range_img(gfx, 85, 85, 85, 148, 148, 148);
	focus_img  = gen_range_img(gfx, 85, 85, 85, 162, 162, 162);
	readonly_img  = gen_range_img(gfx, 64, 64, 64, 128, 128, 128);

	/* define general widget functions */
	widman->default_widget_methods(&gen_methods);

	orig_updatepos    = gen_methods.updatepos;
	orig_handle_event = gen_methods.handle_event;

	gen_methods.get_type     = entry_get_type;
	gen_methods.draw         = entry_draw;
	gen_methods.updatepos    = entry_updatepos;
	gen_methods.handle_event = entry_handle_event;
	gen_methods.calc_minmax  = entry_calc_minmax;
	gen_methods.find         = entry_find;
	gen_methods.free_data    = entry_free_data;

	build_script_lang();

	d->register_module("Entry 1.0", &services);
	return 1;
}
