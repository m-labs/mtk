/*
 * Copyright (C) 2005-2008 Norman Feske <norman.feske@genode-labs.com>
 * Genode Labs, Feske & Helmuth Systementwicklung GbR
 * Copyright (C) 2010, 2011 Sebastien Bourdeauducq
 *
 * This file is part of the MTK-embedded package, which is distributed
 * under the terms of the GNU General Public License version 2.
 */

/**
 * General includes
 */

#include <assert.h>

/**
 * Local includes
 */
#include "mtkstd.h"
#include "scrdrv.h"
#include "clipping.h"

static int scr_width, scr_height, scr_depth;
static int curr_mx = 100,curr_my = 100;
static struct clipping_services *clip;

static short *scr;
static short buf[1024*768];

static void draw_cursor(short *data,int x,int y)
{
	static int i,j;
	short *dst = (short *)buf + y*scr_width + x;
	short *d;
	short *s = data;
	int w = *(data++), h = *(data++);
	int linelen = w;
	if (x >= scr_width) return;
	if (y >= scr_height) return;
	if (x >= scr_width  - 16) linelen = scr_width - x;
	if (y >= scr_height - 16) h = scr_height - y;
	for (j = 0; j < h; j++) {
		d = dst; s = data;
		for (i = 0; i < linelen; i++) {
			if (*s) *d = *s;
			d++; s++;
		}
		dst  += scr_width;
		data += w;
	}
}


static short bg_buffer[20][16];

static void save_background(int x, int y)
{
	short *src = (short *)buf + y*scr_width + x;
	short *dst = (short *)&bg_buffer;
	short *s;
	static int i, j;
	int h = 16;
	if (y >= scr_height - 16) h = scr_height - y;
	for (j = 0; j < h; j++) {
		s = src;
		for (i = 0; i < 16; i++) {
			*(dst++) = *(s++);
		}
		src += scr_width;
	}
}

static void restore_background(int x, int y)
{
	short *src = (short *)&bg_buffer;
	short *dst = (short *)buf + y*scr_width + x;
	short *d;
	static int i, j;
	int h = 16;
	if (y >= scr_height - 16) h = scr_height - y;
	for (j = 0; j < h; j++) {
		d = dst;
		for (i = 0; i < 16; i++) {
			*(d++) = *(src++);
		}
		dst += scr_width;
	}
}

extern short bigmouse_trp;

/***********************
 ** Service functions **
 ***********************/

/**
 * Set up screen
 */
static int set_screen(void *fb, int width, int height, int depth)
{
	int i;

	scr_width  = width;
	scr_height = height;
	scr_depth  = 16;
	scr = (short int *)fb;

	for(i=0;i<scr_width*scr_height;i++) {
		scr[i] = 0;
		buf[i] = 0;
	}

	return 1;
}

/**
 * Deinitialisation
 */
static void restore_screen(void)
{
}


/**
 * Provide information about the screen
 */
static int  get_scr_width  (void) {return scr_width;}
static int  get_scr_height (void) {return scr_height;}
static int  get_scr_depth  (void) {return scr_depth;}
static void *get_scr_adr    (void) {return &scr;}
static void *get_buf_adr    (void) {return &buf;}


/**
 * Make changes on the screen visible (buffered output)
 */

static void update_area(int x1, int y1, int x2, int y2)
{
	int dx;
	int dy;
	int v;
	int i, j;
	u16 *src, *dst;
	u32 *s, *d;
	int cursor_visible = 0;

	if ((curr_mx < x2) && (curr_mx + 16 > x1)
	 && (curr_my < y2) && (curr_my + 16 > y1)) {
		save_background(curr_mx, curr_my);
		draw_cursor(&bigmouse_trp, curr_mx, curr_my);
		cursor_visible = 1;
	}

	x1 &= ~3;
	x2 = (x2 | 3) + 1;

	/* apply clipping to specified area */
	if (x1 < (v = clip->get_x1())) x1 = v;
	if (y1 < (v = clip->get_y1())) y1 = v;
	if (x2 > (v = clip->get_x2())) x2 = v;
	if (y2 > (v = clip->get_y2())) y2 = v;

	dx = x2 - x1;
	dy = y2 - y1;

	if (dx >= 0 && dy >= 0) {

		/* determine offset of left top corner of the area to update */
		src = (u16 *)buf + y1*scr_width + x1;
		dst = (u16 *)scr + y1*scr_width + x1;

		for (j = dy + 1; j--; ) {

			/* copy line */
			d = (u32 *)dst; s = (u32 *)src;
			for (i = (dx>>1) + 2; i--; ) *(d++) = *(s++);

			src += scr_width;
			dst += scr_width;
		}
	}

	if (cursor_visible)
		restore_background(curr_mx, curr_my);
}

/**
 * Set mouse cursor to the specified position
 */
static void set_mouse_pos(int mx, int my)
{
	int old_mx = curr_mx, old_my = curr_my;

	/* check if position really changed */
	if ((curr_mx == mx) && (curr_my == my)) return;

	curr_mx = mx;
	curr_my = my;
	update_area(curr_mx, curr_my, curr_mx + 16, curr_my + 16);
	update_area(old_mx,  old_my,  old_mx  + 16, old_my  + 16);
}

/**
 * Set new mouse shape
 */
static void set_mouse_shape(void *new_shape)
{
	/* not built in yet... */
}


/**************************************
 ** Service structure of this module **
 **************************************/

static struct scrdrv_services services = {
	set_screen,
	restore_screen,
	get_scr_width,
	get_scr_height,
	get_scr_depth,
	get_scr_adr,
	get_buf_adr,
	update_area,
	set_mouse_pos,
	set_mouse_shape,
};

/************************
 ** Module entry point **
 ************************/

int init_scrdrv(struct mtk_services *d)
{
	/* define frame buffer address */

	clip = d->get_module("Clipping 1.0");
	d->register_module("ScreenDriver 1.0", &services);
	return 1;
}
