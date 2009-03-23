/*
 * \brief   DOpE command terminal
 * \date    2002-11-21
 * \author  Norman Feske
 *
 * This is a small example client for DOpE. It provides
 * a terminal to interact with the DOpE server based on
 * the DOpE command language.
 */

/*
 * Copyright (C) 2002-2009 Norman Feske
 * Genode Labs, Feske & Helmuth Systementwicklung GbR
 *
 * This file is part of the Genode OS framework, which is distributed
 * under the terms of the GNU General Public License version 2.
 */

/* general includes */
#include <stdio.h>
#include <string.h>

/* DOpE includes */
#include <dopelib.h>
#include <vscreen.h>
#include <keycodes.h>

/* local includes */
#include "dopecmd.h"

#define TERM_W 80
#define TERM_H 25

static long  app_id;
static int   curs_x, curs_y;   /* cursor position              */
static char *cbuf_adr;         /* VTextScreen character buffer */
static char *abuf_adr;         /* VTextScreen attribute buffer */


/**
 * Utility: set vtextscreen cursor to new position
 */
static void set_cursor(int x, int y) {

	/* take over new cursor position */
	curs_x = x; curs_y = y;
	dope_cmdf(app_id, "vts.set(-cursorx %d -cursory %d)", curs_x, curs_y);
}


/**
 * Utility: scroll vtextscreen content one line up
 */
static void scroll(void) {

	/* move visible lines up */
	memmove(cbuf_adr, cbuf_adr + TERM_W, TERM_W*(TERM_H - 1));
	memmove(abuf_adr, abuf_adr + TERM_W, TERM_W*(TERM_H - 1));

	/* clear new line */
	memset(cbuf_adr + TERM_W*(TERM_H - 1), 0, 80);
	memset(abuf_adr + TERM_W*(TERM_H - 1), 0, 80);

	/* refresh vtextscreen */
	dope_cmd(app_id, "vts.refresh()");
}


/**
 * Utility: insert character at cursor position
 */
static void insert_char(char c) {
	char *cadr = cbuf_adr + curs_y*TERM_W + curs_x;
	char *aadr = abuf_adr + curs_y*TERM_W + curs_x;
	int len = strlen(cadr);

	if (curs_x >= TERM_W - 1) return;

	/* move all characters behind the cursor to the right */
	memmove(cadr + 1, cadr, len);
	memmove(aadr + 1, aadr, len);

	/* insert character into vtextscreen buffer */
	cbuf_adr[curs_y*TERM_W + curs_x] = c;
	abuf_adr[curs_y*TERM_W + curs_x] = (7<<3) + (3<<6);

	/* refresh vtextscreen */
	dope_cmdf(app_id, "vts.refresh(-x %d -y %d -w %d -h 1)", curs_x, curs_y, len + 1);

	/* move cursor to right */
	set_cursor(curs_x + 1, curs_y);
}


/**
 * Utility: delete character at cursor position
 */
static void delete_char(void) {
	char *cadr = cbuf_adr + curs_y*TERM_W + curs_x;
	char *aadr = abuf_adr + curs_y*TERM_W + curs_x;
	int len = strlen(cadr) + 1;

	if (curs_x <= 5) return;

	/* move cursor to the left */
	set_cursor(curs_x - 1, curs_y);

	/* move all characters behind the cursor to left */
	memmove(cadr - 1, cadr, len);
	memmove(aadr - 1, aadr, len);
	dope_cmdf(app_id, "vts.refresh(-x %d -y %d -w %d -h 1)", curs_x, curs_y, len);
}


/**
 * Utility: print DOpE command prompt
 */
static void print_prompt(void) {
	sprintf(cbuf_adr + curs_y*TERM_W, "DOpE>");
	memset(abuf_adr + curs_y*TERM_W, (7<<3) + (0<<6) + 1, 5);
	set_cursor(5, curs_y);
	dope_cmdf(app_id, "vts.refresh(-x %d -y %d -w 5 -h 1)", curs_x, curs_y);
}


/**
 * Utility: insert new line
 */
static void newline(void) {
	if (curs_y < TERM_H - 1) {
		set_cursor(0, curs_y + 1);
	} else {
		scroll();
		set_cursor(0, curs_y);
	}
}


/**
 * Utility: print result string
 */
static void print_result(char *res) {
	int len = strlen(res);

	/* cut too large strings to fit on the current line */
	if (curs_x + len > TERM_W) len = TERM_W - curs_x;

	/* printf string to character buffer at current cursor position */
	snprintf(cbuf_adr + curs_y*TERM_W + curs_x, len + 1, "%s", res);

	/* use yellow text on black background */
	memset(abuf_adr + curs_y*TERM_W, (3<<3) + (3<<6), len);

	/* set cursor behind the string */
	set_cursor(curs_x + len, curs_y);
}


/**
 * Callback: called for each key stroke
 */
static void press_callback(dope_event *e,void *arg) {
	static char res[256];
	char nc;

	if ((e->type != EVENT_TYPE_PRESS)
	 && (e->type != EVENT_TYPE_KEYREPEAT)) return;

	nc = dope_get_ascii(app_id, e->press.code);

	/* move cursor to left but prevent overwriting the prompt */
	if (e->press.code == DOPE_KEY_LEFT) {
		if (curs_x > 5) set_cursor(curs_x - 1, curs_y);
		return;
	}

	/* move cursor to right but only in string range */
	if (e->press.code == DOPE_KEY_RIGHT) {
		if (cbuf_adr[curs_y*TERM_W + curs_x]) set_cursor(curs_x + 1, curs_y);
		return;
	}

	/* only handle valid ascii characters */
	if (!nc) return;

	switch (nc) {

		/* backspace */
		case 8:
			delete_char();
			break;

		/* return */
		case 10:

			/* if there are some characters on the line, execute them as command */
			if (cbuf_adr[curs_y*TERM_W + 5]) {
				dope_reqf(app_id, res, sizeof(res), "%s", cbuf_adr + curs_y*TERM_W + 5);
				newline();
				print_result(res);
			}
			newline();
			print_prompt();
			break;

		/* other printable character */
		default:
			insert_char(nc);
			break;
	}
}


/**
 * Callback: called when the window-close button got pressed
 */
static void close_callback(dope_event *e,void *arg)
{
	dope_cmd(app_id, "termw.close()");
}


/*************************
 ** Interface functions **
 *************************/

void init_dopecmd(void)
{
	app_id = dope_init_app("DOpEcmd");

	dope_cmd_seq(app_id,

		/* create some buttons */
		"conn    = new Button(-text \"Connect\")",
		"disconn = new Button(-text \"Disconnect\")",
		"clear   = new Button(-text \"Clear\")",
		"save    = new Button(-text \"Save...\")",

		/* place buttons into a grid */
		"bg = new Grid()",
		"bg.place(conn,    -column 1 -row 1)",
		"bg.place(disconn, -column 2 -row 1)",
		"bg.place(clear,   -column 3 -row 1)",
		"bg.place(save,    -column 4 -row 1)",
		"bg.columnconfig(5, -weight 1)",

		/* add a vtextscreen widget that resides in a scrollable frame */
		"vts = new VTextScreen()",
		"tf  = new Frame(-scrollx yes -scrolly yes -content vts)",

		/* place button grid and terminal frame into a grid with two rows */
		"g = new Grid()",
		"g.place(bg,-column 1 -row 1)",
		"g.place(tf,-column 1 -row 2)",

		/* create and open the dope command window */
		"termw = new Window(-content g -x 200 -y 150 -w 430 -h 365)",
		0
	);

	/* set mode of vtextscreen */
	dope_cmdf(app_id, "vts.setmode(%d, %d, C8A8PLN)", TERM_W, TERM_H);

	/* map vtextscreen buffer to local address space */
	cbuf_adr = vscr_get_fb(app_id, "vts");
	abuf_adr = cbuf_adr + TERM_W*TERM_H;

	if (!cbuf_adr) {
		printf("Error: could not map vtextscreen buffer\n");
		return;
	}

	print_prompt();

	dope_bind(app_id, "vts", "press",     press_callback, (void *)0xaaa);
	dope_bind(app_id, "vts", "keyrepeat", press_callback, (void *)0xaaa);
	dope_bind(app_id, "termw", "close", close_callback, (void *)123);
}


void open_dopecmd_window(void)
{
	dope_cmd(app_id, "termw.open()");
	dope_cmd(app_id, "termw.top()");
}
