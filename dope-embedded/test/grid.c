/*
 * \brief  DOpE-embedded demo application (grid demo)
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

/* DOpE includes */
#include <dopelib.h>
#include <vscreen.h>
#include <stdio.h>
#include <stdlib.h>

/* local includes */
#include "settings.h"
#include "util.h"
#include "gfx/genode-labs-banner.h"


static int app_id;  /* DOpE application ID used for the grid demo */


/**
 * Callback: called when the vertical scale widget was changed
 */
void vscale_callback(dope_event *e, void *arg)
{
	/* request new scale value */
	int v = dope_req_l(app_id, "vscale.value");

	/* adjust grid weights */
	dope_cmdf(app_id, "table.rowconfig(1, -weight %d)", v);
	dope_cmdf(app_id, "table.rowconfig(2, -weight %d)", 100 - v);
}


/**
 * Callback: called when the horizontal scale widget was changed
 */
void hscale_callback(dope_event *e, void *arg)
{
	/* request new scale value */
	int v = dope_req_l(app_id, "hscale.value");

	/* adjust grid weights */
	dope_cmdf(app_id, "table.columnconfig(1, -weight %d)", v);
	dope_cmdf(app_id, "table.columnconfig(2, -weight %d)", 100 - v);
}


/**
 * Callback: called when window-close button got pressed
 */
static void close_callback(dope_event *e, void *arg)
{
	dope_cmd(app_id, "w.close()");
}


/*************************
 ** Interface functions **
 *************************/

void init_grid_window(void)
{
	short *pixbuf;
	const char *pixels = pixel_data_genode;
	int w = 232, h = 84, i;

	app_id = dope_init_app("Grid demo");

	/* create vscreen widget holding a pixel buffer */
	dope_cmd (app_id, "vs = new VScreen()");
	dope_cmdf(app_id, "vs.setmode(%d, %d, RGB16)", w, h);
	pixbuf = (short *)vscr_get_fb(app_id, "vs");
	if (!pixbuf) {
		printf("Error: could not map vs\n");
	}

	/* convert 32bit source pixels to 16bit RGBA pixels as by for the vscr widget */
	for (i = 0; i < w*h; i++) {
		unsigned char pixel[3];
		HEADER_PIXEL(pixels, pixel);
		pixbuf[i] = ((pixel[0] >> 3) << 11) | ((pixel[1] >> 2) << 5) | (pixel[2] >> 3);
	}

	dope_cmd_seq(app_id,
		"table = new Grid()",
		"vs_tl = new VScreen()",
		"vs_tr = new VScreen()",
		"vs_bl = new VScreen()",
		"vs_br = new VScreen()",
		"vs_tl.share(vs)",
		"vs_tr.share(vs)",
		"vs_bl.share(vs)",
		"vs_br.share(vs)",
		"f_tl = new Frame(-scrollx no -scrolly no -content vs_tl)",
		"f_tr = new Frame(-scrollx no -scrolly no -content vs_tr)",
		"f_bl = new Frame(-scrollx no -scrolly no -content vs_bl)",
		"f_br = new Frame(-scrollx no -scrolly no -content vs_br)",
		"table.place(f_tl, -row 1 -column 1)",
		"table.place(f_tr, -row 1 -column 2)",
		"table.place(f_bl, -row 2 -column 1)",
		"table.place(f_br, -row 2 -column 2)",
		"vscale = new Scale(-from 1 -to 99 -value 50 -orient vertical)",
		"hscale = new Scale(-from 1 -to 99 -value 50 -orient horizontal)",
		"g = new Grid()",
		"g.place(hscale, -row 1 -column 2)",
		"g.place(vscale, -row 2 -column 1)",
		"g.place(table, -row 2 -column 2)",
		"w = new Window(-content g -workx 400 -worky 200 -workw 478 -workh 182)",
		0
	);

	dope_bind(app_id, "vscale", "change", vscale_callback, (void *)0);
	dope_bind(app_id, "hscale", "change", hscale_callback, (void *)0);
	dope_bind(app_id, "w",      "close",  close_callback,  (void *)0);
}


void open_grid_window(void)
{
	/* reset window and grid geometry */
	dope_cmd(app_id, "w.set(-workw 478 -workh 182)");
	dope_cmd(app_id, "vscale.set(-value 50)");
	dope_cmd(app_id, "hscale.set(-value 50)");
	dope_cmd(app_id, "table.columnconfig(1, -weight 50)");
	dope_cmd(app_id, "table.columnconfig(2, -weight 50)");

	dope_cmd(app_id, "w.open()");
	dope_cmd(app_id, "w.top()");
}
