/*
 * \brief  DOpE gfx module
 *
 * This module contains the implementation of the
 * graphics hardware abstraction interface. It
 * provides functions to manage the screens, the
 * allocation of data types, basic drawing functions
 * and mouse pointer handling.
 */

/*
 * Copyright (C) 2003-2008 Norman Feske <norman.feske@genode-labs.com>
 * Genode Labs, Feske & Helmuth Systementwicklung GbR
 *
 * This file is part of the DOpE-embedded package, which is distributed
 * under the terms of the GNU General Public License version 2.
 */

#include "dopestd.h"
#include "gfx_handler.h"
#include "gfx.h"

static struct gfx_handler_services *gfxscr_rgb16;
static struct gfx_handler_services *gfximg_rgb16;
static struct gfx_handler_services *gfximg_rgba32;

static struct gfx_ds_handler gfxscr_rgb16_handler;
static struct gfx_ds_handler gfximg_rgb16_handler;
static struct gfx_ds_handler gfximg_rgba32_handler;

int init_gfx(struct dope_services *d);


/***********************
 ** Private functions **
 ***********************/

/**
 * Fill handler structure with dummy default callbacks
 */
static void *dummy(void)
{ return NULL; }
static void set_handler_defaults(struct gfx_ds_handler *handler)
{
	handler->get_width        = (void *)dummy;
	handler->get_height       = (void *)dummy;
	handler->get_type         = (void *)dummy;
	handler->destroy          = (void *)dummy;
	handler->map              = (void *)dummy;
	handler->unmap            = (void *)dummy;
	handler->update           = (void *)dummy;
	handler->get_ident        = (void *)dummy;
	handler->draw_hline       = (void *)dummy;
	handler->draw_vline       = (void *)dummy;
	handler->draw_fill        = (void *)dummy;
	handler->draw_slice       = (void *)dummy;
	handler->draw_img         = (void *)dummy;
	handler->draw_string      = (void *)dummy;
	handler->push_clipping    = (void *)dummy;
	handler->pop_clipping     = (void *)dummy;
	handler->reset_clipping   = (void *)dummy;
	handler->get_clip_x       = (void *)dummy;
	handler->get_clip_y       = (void *)dummy;
	handler->get_clip_w       = (void *)dummy;
	handler->get_clip_h       = (void *)dummy;
	handler->set_mouse_cursor = (void *)dummy;
	handler->set_mouse_pos    = (void *)dummy;
}


/*************************
 ** Interface functions **
 *************************/

/**
 * Allocate and initialise gfx dataspace for screen
 */
static struct gfx_ds *alloc_scr(char *scrmode)
{
	struct gfx_ds *new;

	new = zalloc(sizeof(struct gfx_ds));
	if (!new) return NULL;

	new->handler = &gfxscr_rgb16_handler;
	new->ref_cnt = 1;
	new->data = gfxscr_rgb16->create(640, 480, &(new->handler));
	return new;
}


/**
 * Allocate gfx dataspace for images
 */
static struct gfx_ds *alloc_img(int w, int h, enum img_type img_type)
{
	struct gfx_ds *new;

	INFO(printf("Gfx(alloc_img): w=%d, h=%d, type=%d\n", (int)w, (int)h, (int)img_type));
	new = (struct gfx_ds *)zalloc(sizeof(struct gfx_ds));
	if (!new) return NULL;

	switch (img_type) {

	case GFX_IMG_TYPE_RGBA32:
		new->handler = &gfximg_rgba32_handler;
		new->data = gfximg_rgba32->create(w, h, &new->handler);
		break;

	case GFX_IMG_TYPE_RGB16:
		new->handler = &gfximg_rgb16_handler;
		new->data = gfximg_rgb16->create(w, h, &new->handler);
		break;

	default:
		return NULL;
	}

	INFO(printf("Gfx(alloc_img): new->data=%x new->handler=%x\n", (int)new->data, (int)new->handler));
	new->update_cnt = 0;
	new->ref_cnt = 1;
	return new;
}


static int load_fnt(char *fntname)
{
	return 0;
}


/**
 * Forward generic function calls to specific gfx dataspace type handler
 */
static int get_width(struct gfx_ds *ds)
{
	return ds->handler->get_width(ds->data);
}

static int get_height(struct gfx_ds *ds)
{
	return ds->handler->get_height(ds->data);
}

static enum img_type get_type(struct gfx_ds *ds)
{
	return ds->handler->get_type(ds->data);
}

static void inc_ref(struct gfx_ds *ds)
{
	ds->ref_cnt++;
}

static void dec_ref(struct gfx_ds *ds)
{
	ds->ref_cnt--;
	if (ds->ref_cnt > 0) return;
	ds->handler->destroy(ds->data);
	free(ds);
}

static void *map(struct gfx_ds *ds)
{
	return ds->handler->map(ds->data);
}

static void unmap(struct gfx_ds *ds)
{
	ds->handler->unmap(ds->data);
}

static void update(struct gfx_ds *ds, int x, int y, int w, int h)
{
	ds->handler->update(ds->data, x, y, w, h);
	ds->update_cnt++;
}

static int get_upcnt(struct gfx_ds *ds)
{
	return ds->update_cnt;
}

static int get_ident(struct gfx_ds *ds, char *dst_ident)
{
	return ds->handler->get_ident(ds->data, dst_ident);
}

static void draw_hline(struct gfx_ds *ds, int x, int y, int w, color_t rgba)
{
	ds->handler->draw_hline(ds->data, x, y, w, rgba);
}

static void draw_vline(struct gfx_ds *ds, int x, int y, int h, color_t rgba)
{
	ds->handler->draw_vline(ds->data, x, y, h, rgba);
}

static void draw_fill(struct gfx_ds *ds, int x, int y, int w, int h, color_t rgba)
{
	ds->handler->draw_fill(ds->data, x, y, w, h, rgba);
}

static void draw_slice(struct gfx_ds *ds, int x,  int y,  int w,  int h,
                       int sx, int sy, int sw, int sh,
                       struct gfx_ds *img, u8 alpha) {
	ds->handler->draw_slice(ds->data, x, y, w, h, sx, sy, sw, sh, img, alpha);
}

static void draw_img(struct gfx_ds *ds, int x, int y, int w, int h,
                     struct gfx_ds *img, u8 alpha) {
	ds->handler->draw_img(ds->data, x, y, w, h, img, alpha);
}

static void draw_string(struct gfx_ds *ds, int x, int y, color_t fg_rgba, color_t bg_rgba,
                        int font_id, char *str) {
	ds->handler->draw_string(ds->data, x, y , fg_rgba, bg_rgba, font_id, str);
}

static void push_clipping(struct gfx_ds *ds, int x, int y, int w, int h)
{
	ds->handler->push_clipping(ds->data, x, y, w, h);
}

static void pop_clipping(struct gfx_ds *ds)
{
	ds->handler->pop_clipping(ds->data);
}

static void reset_clipping(struct gfx_ds *ds)
{
	ds->handler->reset_clipping(ds->data);
}

static int get_clip_x(struct gfx_ds *ds)
{
	return ds->handler->get_clip_x(ds->data);
}

static int get_clip_y(struct gfx_ds *ds)
{
	return ds->handler->get_clip_y(ds->data);
}

static int get_clip_w(struct gfx_ds *ds)
{
	return ds->handler->get_clip_w(ds->data);
}

static int get_clip_h(struct gfx_ds *ds)
{
	return ds->handler->get_clip_h(ds->data);
}

static void set_mouse_cursor(struct gfx_ds *ds, struct gfx_ds *cursor)
{
	ds->handler->set_mouse_cursor(ds->data, cursor);
}

static void set_mouse_pos(struct gfx_ds *ds, int x, int y)
{
	ds->handler->set_mouse_pos(ds->data, x, y);
}


/**************************************
 ** Service structure of this module **
 **************************************/

static struct gfx_services services = {
	alloc_scr,     alloc_img,  load_fnt,
	get_width,     get_height, get_type,
	inc_ref,       dec_ref,
	map,           unmap,      update,
	get_ident,
	get_upcnt,
	draw_hline,    draw_vline, draw_fill,
	draw_slice,    draw_img,   draw_string,
	push_clipping,
	pop_clipping,
	reset_clipping,
	get_clip_x,    get_clip_y, get_clip_w,  get_clip_h,
	set_mouse_cursor, set_mouse_pos
};


/************************
 ** Module entry point **
 ************************/

int init_gfx(struct dope_services *d)
{
	gfxscr_rgb16  = d->get_module("GfxScreen16 1.0");
	gfximg_rgb16  = d->get_module("GfxImage16 1.0");
	gfximg_rgba32 = d->get_module("GfxImage32 1.0");

	set_handler_defaults(&gfxscr_rgb16_handler);
	gfxscr_rgb16->register_gfx_handler(&gfxscr_rgb16_handler);

	set_handler_defaults(&gfximg_rgba32_handler);
	gfximg_rgba32->register_gfx_handler(&gfximg_rgba32_handler);

	set_handler_defaults(&gfximg_rgb16_handler);
	gfximg_rgb16->register_gfx_handler(&gfximg_rgb16_handler);

	d->register_module("Gfx 1.0", &services);
	return 1;
}
