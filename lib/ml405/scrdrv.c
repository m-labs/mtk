/*
 * \brief   DOpE screen driver module for Charon's FPGA board
 * \date    2005-02-04
 * \author  Norman Feske <norman.feske@genode-labs.com>
 */

/*
 * Copyright (C) 2005-2008 Norman Feske <norman.feske@genode-labs.com>
 * Genode Labs, Feske & Helmuth Systementwicklung GbR
 *
 * This file is part of the DOpE-embedded package, which is distributed
 * under the terms of the GNU General Public License version 2.
 */

/**
 * General includes
 */
#include <signal.h>
//#include <stdio.h>

/**
 * FPGA includes
 */
#include "xparameters.h"
#include "xio.h"
#ifdef __PPC__
#include "xcache_l.h"
#endif /* __PPC__ */

/**
 * Local includes
 */
#include "dopestd.h"
#include "scrdrv.h"
#include "clipping.h"

static long scr_width, scr_height, scr_depth;
static void *scr_adr, *buf_adr;
static long curr_mx = 100,curr_my = 100;
static struct clipping_services *clip;

int init_scrdrv(struct dope_services *d);


static void draw_cursor(short *data,long x,long y)
{
	static int i,j;
	short *dst = (short *)buf_adr + y*scr_width + x;
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

static void save_background(long x, long y)
{
	short *src = (short *)buf_adr + y*scr_width + x;
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


static void restore_background(long x, long y)
{
	short *src = (short *)&bg_buffer;
	short *dst = (short *)buf_adr + y*scr_width + x;
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


extern short smallmouse_trp;
extern short bigmouse_trp;


/***********************
 ** Service functions **
 ***********************/


/**
 * Set up screen
 */
static long set_screen(long width, long height, long depth)
{
	int scr_size;
	int baseaddr = XPAR_PLB_NPI_VGA_CONTROLLER_0_BASEADDR;
	int scr_adr_int;

	/****************************************
	 * 25 MHz Pixel clock: 640x480 at 60 Hz *
	 ****************************************/
//	scr_width  = 640;
//	scr_height = 480;
//	scr_depth  = 16;
//
//	scr_size = scr_width*(scr_height + 16)*scr_depth/8 + 64;
//	scr_adr  = malloc(scr_size);
//	buf_adr  = malloc(scr_size);
//
//	/* align screen address to a 128-byte-aligned address */
//	scr_adr_int = (int) scr_adr;
//	scr_adr_int += 128;
//	scr_adr_int &= 0xFFFFFF80;
//	scr_adr = (void *)scr_adr_int;
//
//	/* set frame buffer address */
//	*((volatile u32 **)(baseaddr)) = scr_adr;
//
//	/* set screen resolution to 640x480 */
//	*((volatile u32 *)(baseaddr + 0x14)) = (scr_height << 16) + scr_width;
//
//	/* set frame buffer size in bytes */
//	*((volatile u32 *)(baseaddr + 0x18)) = scr_height * scr_width * 2;
//
//	/* set hsync start and hsync end */
//	*((volatile u32 *)(baseaddr + 0x1C)) = ((640+28+94) << 16) + 640+28;
//
//	/* set vsync start and vsync end */
//	*((volatile u32 *)(baseaddr + 0x20)) = (491 << 16) + 490;
//
//	/* set x and y max */
//	*((volatile u32 *)(baseaddr + 0x24)) = (524 << 16) + 792;
//
//	/* set bgnd low and bgnd high */
//	*((volatile u32 *)(baseaddr + 0x28)) = (505 << 16) + 490;
//
//	/* set normal resolution and enable frame buffer */
//	*((volatile u32 *)(baseaddr + 0x10)) = 4;
//
	/*****************************************
	 * 50 MHz Pixel clock: 800x600 at 72 Hz  *
	 *****************************************/
	scr_width  = 800;
	scr_height = 600;
	scr_depth  = 16;

	scr_size = scr_width*(scr_height + 16)*scr_depth/8 + 128;
	scr_adr  = malloc(scr_size);
	buf_adr  = malloc(scr_size);

	/* align screen address to a 128-byte-aligned address */
	scr_adr_int = (int) scr_adr;
	scr_adr_int += 128;
	scr_adr_int &= 0xFFFFFF80;
	scr_adr = (void *)scr_adr_int;

	/* set frame buffer address */
	*((volatile u32 **)(baseaddr)) = scr_adr;

	/* set screen resolution to 800x600 */
	*((volatile u32 *)(baseaddr + 0x14)) = (scr_height << 16) + scr_width;

	/* set frame buffer size in bytes */
	*((volatile u32 *)(baseaddr + 0x18)) = scr_height * scr_width * 2;

	/* set hsync start and hsync end */
	*((volatile u32 *)(baseaddr + 0x1C)) = ((800+56+120) << 16) + 800+56;

	/* set vsync start and vsync end */
	*((volatile u32 *)(baseaddr + 0x20)) = (641 << 16) + 636;

	/* set x and y max */
	*((volatile u32 *)(baseaddr + 0x24)) = ((600+66-1) << 16) + 800+239;

	/* set bgnd low and bgnd high */
	*((volatile u32 *)(baseaddr + 0x28)) = (660 << 16) + 630;

	/* set normal resolution and enable frame buffer */
	*((volatile u32 *)(baseaddr + 0x10)) = 4;


	/*****************************************
	 * 75 MHz Pixel clock: 1024x768 at 60 Hz *
	 *****************************************/
//	scr_width  = 1024;
//	scr_height = 768;
//	scr_depth  = 16;
//
//	scr_size = scr_width*(scr_height + 16)*scr_depth/8 + 128;
//	scr_adr  = malloc(scr_size);
//	buf_adr  = malloc(scr_size);
//
//	/* align screen address to a 128-byte-aligned address */
//	scr_adr_int = (int) scr_adr;
//	scr_adr_int += 128;
//	scr_adr_int &= 0xFFFFFF80;
//	scr_adr = (void *)scr_adr_int;
//
//	/* set frame buffer address */
//	*((volatile u32 **)(baseaddr)) = scr_adr;
//
//	/* set screen resolution to 1024x768 */
//	*((volatile u32 *)(baseaddr + 0x14)) = (scr_height << 16) + scr_width;
//
//	/* set frame buffer size in bytes */
//	*((volatile u32 *)(baseaddr + 0x18)) = scr_height * scr_width * 2;
//
//	/* set hsync start and hsync end */
//	*((volatile u32 *)(baseaddr + 0x1C)) = ((1024+16+224) << 16) + 640+16;
//
//	/* set vsync start and vsync end */
//	*((volatile u32 *)(baseaddr + 0x20)) = (783 << 16) + 780;
//
//	/* set x and y max */
//	*((volatile u32 *)(baseaddr + 0x24)) = (858 << 16) + 1455;
//
//	/* set bgnd low and bgnd high */
//	*((volatile u32 *)(baseaddr + 0x28)) = (850 << 16) + 779;
//
//	/* set normal resolution and enable frame buffer */
//	*((volatile u32 *)(baseaddr + 0x10)) = 4;


//	printf("ScrDrv(set_screen): scr_adr=%p, buf_adr=%p\n", scr_adr, buf_adr);
//	printf("Baseaddr=%p\n", baseaddr);
//	printf("Framebuffer_addr=%p\n", *((u32 *)(baseaddr)));
//	printf("resolution=%p\n", *((u32 **)(baseaddr+0x14)));
//	printf("Frame Buffer size=%p\n", *((u32 **)(baseaddr+0x18)));
//	printf("hsync size=%p\n", *((u32 **)(baseaddr+0x1C)));
//	printf("vsync size=%p\n", *((u32 **)(baseaddr+0x20)));
//	printf("x/y max=%p\n", *((u32 **)(baseaddr+0x24)));
//	printf("bgnd high/low=%p\n", *((u32 **)(baseaddr+0x28)));
//	printf("flags=%p\n", *((u32 **)(baseaddr+0x10)));
	return 1;
}


/**
 * Deinitialisation
 */
static void restore_screen(void)
{
	/* nothing to restore - sdl is too good */
}


/**
 * Provide information about the screen
 */
static long  get_scr_width  (void) {return scr_width;}
static long  get_scr_height (void) {return scr_height;}
static long  get_scr_depth  (void) {return scr_depth;}
static void *get_scr_adr    (void) {return scr_adr;}
static void *get_buf_adr    (void) {return buf_adr;}


/**
 * Make changes on the screen visible (buffered output)
 */
static void update_area(long x1, long y1, long x2, long y2)
{
	long dx;
	long dy;
	long v;
	long i, j;
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
		src = (u16 *)buf_adr + y1*scr_width + x1;
		dst = (u16 *)scr_adr + y1*scr_width + x1;

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

	XCache_FlushDCacheRange((int)scr_adr + y1*scr_width + x1, 2*scr_width*scr_height);
}


/**
 * Set mouse cursor to the specified position
 */
static void set_mouse_pos(long mx, long my)
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

int init_scrdrv(struct dope_services *d)
{
	/* define frame buffer address */

	clip = d->get_module("Clipping 1.0");
	d->register_module("ScreenDriver 1.0", &services);
	return 1;
}
