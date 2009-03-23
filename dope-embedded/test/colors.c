/*
 * \brief  DOpE-embedded demo application (diplay image of 4096 colors)
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

#include "colors.h"

static int app_id;


static void close_callback(dope_event *e, void *arg)
{
	dope_cmd(app_id, "w.close()");
}


/*************************
 ** Interface functions **
 *************************/

int init_colors_window(void)
{
	int i;
	int blue = 0, green = 0, red = 0;
	short *pixbuf;

	app_id = dope_init_app("4096 Colors");

	dope_cmd_seq(app_id,
		"vs = new VScreen()",
		"vs.setmode(64, 64, RGB16)",
		"gr = new Grid()",
		"gr.place(vs, -column 1 -row 1)",
		"w = new Window(-content vs -workx 570 -worky 450 -workw 128 -workh 128)",
		(void *)0);

	pixbuf = (short *)vscr_get_fb(app_id, "vs");

	for (i = 0; i < 64*64; i++) {
		pixbuf[i] = red << 11 | green << 5 | blue;
		if (blue == 30) {
			blue = 0;
			green += 4;
		} else
			blue += 2;

		if (green == 64) {
			green = 0;
			red += 2;
		}
	}

	dope_bind(app_id, "w", "close", close_callback, (void *)0);
	return app_id;
}


void open_colors_window(int app_id)
{
	/* reset window geometry */
	dope_cmd(app_id, "w.set(-workw 128 -workh 128)");
	dope_cmd(app_id, "w.open()");
	dope_cmd(app_id, "w.top()");
}
