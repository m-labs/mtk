/*
 * \brief  DOpE-embedded demo application (slideshow)
 * \author Norman Feske
 * \date   2009-03-09
 */

/*
 * Copyright (C) 2009 Norman Feske
 * Genode Labs, Feske & Helmuth Systementwicklung GbR
 *
 * This file is part of the Genode OS framework, which is distributed
 * under the terms of the GNU General Public License version 2.
 */

#include <dopelib.h>
#include <vscreen.h>

#include "slides.h"


/**
 * Size of slides in pixels
 */
enum { SLIDE_W = 500, SLIDE_H = 375 };


/**
 * List of pointers to the pixel data of the slides
 */
static unsigned char *slide_list[] = {
	data_slides_0_rle,
	data_slides_1_rle,
};


static int    curr_slide;   /* currently displayed slide, starting at 0   */
static short *pixbuf;       /* RGB565 buffer used for VScreen widget      */
static int    app_id;       /* DOpE application ID used for the slideshow */


/**
 * Callback: called when window close button got pressed
 */
static void close_callback(dope_event *e, void *arg)
{
	dope_cmd(app_id, "w.close()");
}


/**
 * Return total number of slides
 */
static inline int num_slides(void)
{
	return sizeof(slide_list)/sizeof(unsigned char *);
}


/**
 * Dithering matrix
 */
static const int dither_matrix[16][16] = {
  {   0,192, 48,240, 12,204, 60,252,  3,195, 51,243, 15,207, 63,255 },
  { 128, 64,176,112,140, 76,188,124,131, 67,179,115,143, 79,191,127 },
  {  32,224, 16,208, 44,236, 28,220, 35,227, 19,211, 47,239, 31,223 },
  { 160, 96,144, 80,172,108,156, 92,163, 99,147, 83,175,111,159, 95 },
  {   8,200, 56,248,  4,196, 52,244, 11,203, 59,251,  7,199, 55,247 },
  { 136, 72,184,120,132, 68,180,116,139, 75,187,123,135, 71,183,119 },
  {  40,232, 24,216, 36,228, 20,212, 43,235, 27,219, 39,231, 23,215 },
  { 168,104,152, 88,164,100,148, 84,171,107,155, 91,167,103,151, 87 },
  {   2,194, 50,242, 14,206, 62,254,  1,193, 49,241, 13,205, 61,253 },
  { 130, 66,178,114,142, 78,190,126,129, 65,177,113,141, 77,189,125 },
  {  34,226, 18,210, 46,238, 30,222, 33,225, 17,209, 45,237, 29,221 },
  { 162, 98,146, 82,174,110,158, 94,161, 97,145, 81,173,109,157, 93 },
  {  10,202, 58,250,  6,198, 54,246,  9,201, 57,249,  5,197, 53,245 },
  { 138, 74,186,122,134, 70,182,118,137, 73,185,121,133, 69,181,117 },
  {  42,234, 26,218, 38,230, 22,214, 41,233, 25,217, 37,229, 21,213 },
  { 170,106,154, 90,166,102,150, 86,169,105,153, 89,165,101,149, 85 }
};




/**
 * Convert slide to VScreen buffer
 */
static void display_slide(int i)
{
	unsigned char *src_rle;              /* run-length encoded source data */
	unsigned char  rle_cnt = 0;          /* remaining RLE block size       */
	unsigned char  rle_r, rle_g, rle_b;  /* color of current RLE block     */
	short         *dst;                  /* rgb565 destination buffer      */
	unsigned       r, g, b;              /* current pixel                  */
	int            x, y;                 /* current image position         */

	if (i >= num_slides())
		return;

	src_rle = slide_list[i];
	dst = pixbuf;
	rle_r = rle_g = rle_b = 0;

	for (y = 0; y < SLIDE_H; y++) {
		for (x = 0; x < SLIDE_W; x++) {
			int d = dither_matrix[y & 0xf][x & 0xf] >> 5;

			/* new RLE block starts */
			if (rle_cnt == 0) {
				rle_cnt = *src_rle++;
				rle_r   = *src_rle++;
				rle_g   = *src_rle++;
				rle_b   = *src_rle++;
			}

			/* detect end of rle data */
			if (rle_cnt == 0)
				return;

			rle_cnt--;

			/* apply dithering */
			r = rle_r + d;
			g = rle_g + d;
			b = rle_b + d;

			if (r > 255) r = 255;
			if (g > 255) g = 255;
			if (b > 255) b = 255;

			*dst++ = ((r & 0xf8) << 8) |
			((g & 0xfc) <<  3) |
			((b & 0xf8) >>  3);
		}
	}

	dope_cmd(app_id, "vs.refresh()");
}


/**
 * Names for slideshow controls
 */
enum {
	SLIDE_FIRST,
	SLIDE_PREV,
	SLIDE_NEXT,
	SLIDE_LAST,
};


/**
 * Callback: called when slideshow control was activated
 */
static void slide_control_callback(dope_event *e, void *arg)
{
	switch ((int)arg) {
	case SLIDE_FIRST:
		curr_slide = 0;
		break;
	case SLIDE_PREV:
		if (curr_slide > 0)
			curr_slide--;
		break;
	case SLIDE_NEXT:
		if (curr_slide < num_slides() - 1)
			curr_slide++;
		break;
	case SLIDE_LAST:
		curr_slide = num_slides() - 1;
		break;
	}

	display_slide(curr_slide);
}


/*************************
 ** Interface functions **
 *************************/

void init_slideshow_window(void)
{
	app_id = dope_init_app("Slideshow");

	dope_cmd(app_id, "vs = new VScreen()");
	dope_cmdf(app_id, "vs.setmode(%d, %d, RGB16)", SLIDE_W, SLIDE_H);
	dope_cmd_seq(app_id,

		/* menu grid */
		"mg = new Grid()",
		"b_first = new Button(-text First)",
		"b_prev  = new Button(-text Previous)",
		"b_next  = new Button(-text Next)",
		"b_last  = new Button(-text Last)",
		"mg.place(b_first, -row 1 -column 1)",
		"mg.place(b_prev,  -row 1 -column 2)",
		"mg.place(b_next,  -row 1 -column 3)",
		"mg.place(b_last,  -row 1 -column 4)",

		/* main grid */
		"gr = new Grid()",
		"gr.place(mg, -row 1 -column 1)",
		"gr.place(vs, -row 2 -column 1)",
		(void *)0);

	dope_cmdf(app_id, "gr.rowconfig(2, -size %d)", SLIDE_H);
	dope_cmdf(app_id, "gr.columnconfig(1, -size %d)", SLIDE_W);
	dope_cmdf(app_id, "w = new Window(-content gr -workx 250 -worky 175 -workw %d -workh %d)",
	          SLIDE_W, SLIDE_H);

	pixbuf = (short *)vscr_get_fb(app_id, "vs");

	dope_bind(app_id, "w",       "close",  close_callback,         (void *)0);
	dope_bind(app_id, "b_first", "commit", slide_control_callback, (void *)SLIDE_FIRST);
	dope_bind(app_id, "b_prev",  "commit", slide_control_callback, (void *)SLIDE_PREV);
	dope_bind(app_id, "b_next",  "commit", slide_control_callback, (void *)SLIDE_NEXT);
	dope_bind(app_id, "b_last",  "commit", slide_control_callback, (void *)SLIDE_LAST);
}


void open_slideshow_window(void)
{
	curr_slide = 0;
	display_slide(curr_slide);
	dope_cmd(app_id, "w.open()");
	dope_cmd(app_id, "w.top()");
}
