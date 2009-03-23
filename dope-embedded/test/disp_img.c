/*
 * \brief  DOpE-embedded demo application (diplay static image)
 * \author Matthias Alles
 * \author Norman Feske
 * \date   2008-10-11
 */

/*
 * Copyright (C) 2008 Matthias Alles
 * Genode Labs, Feske & Helmuth Systementwicklung GbR
 *
 * This file is part of the DOpE-embedded package, which is distributed
 * under the terms of the GNU General Public License version 2.
 */

/* generic */
#include <stdio.h>
#include <stdlib.h>

/* dope */
#include <dopelib.h>
#include <vscreen.h>

/* local */
#include "util.h"


void open_image_window(int app_id)
{
	dope_cmd(app_id, "w.open()");
	dope_cmd(app_id, "w.top()");
}


void close_window_callback(dope_event *e, void *arg)
{
	dope_cmd((int)arg, "w.close()");
}


int create_image_window(const char *title, const char *pixels,
                        int x, int y, int w, int h)
{
	int i, app_id = dope_init_app(title);
	short *pixbuf;

	/* create vscreen widget holding a pixel buffer */
	dope_cmd (app_id, "vs = new VScreen()");
	dope_cmdf(app_id, "vs.setmode(%d, %d, RGB16)", w, h);
	pixbuf = (short *)vscr_get_fb(app_id, "vs");
	if (!pixbuf) {
		printf("Error: could not map vs\n");
		return -1;
	}

	/* convert 32bit source pixels to 16bit RGBA pixels as by for the vscr widget */
	for (i = 0; i < w*h; i++) {
		unsigned char pixel[3];
		HEADER_PIXEL(pixels, pixel);
		pixbuf[i] = ((pixel[0] >> 3) << 11) | ((pixel[1] >> 2) << 5) | (pixel[2] >> 3);
	}
	dope_cmd(app_id, "vs.refresh()");

	/* open window displaying the vscreen widget */
	dope_cmd (app_id, "gr = new Grid()");
	dope_cmd (app_id, "gr.place(vs, -column 1 -row 1)");
	dope_cmdf(app_id, "w = new Window(-content vs -workx %d -worky %d -workw %d -workh %d)",
	          x, y, w, h);

	dope_bind(app_id, "w", "close", close_window_callback, (void *)app_id);
	return app_id;
}
