/*
 * \brief   Interface of MTK Screen widget module
 */

/*
 * Copyright (C) 2003-2008 Norman Feske <norman.feske@genode-labs.com>
 * Genode Labs, Feske & Helmuth Systementwicklung GbR
 *
 * This file is part of the MTK package, which is distributed
 * under the terms of the GNU General Public License version 2.
 */

#ifndef _MTK_SCREEN_H_
#define _MTK_SCREEN_H_

#include "widget.h"
#include "gfx.h"

struct screen_methods;
struct screen_data;

#define SCREEN struct screen

struct screen {
	struct widget_methods *gen;
	struct screen_methods *scr;
	struct widget_data    *wd;
	struct screen_data    *sd;
};

struct screen_methods {
	void (*set_gfx)     (SCREEN *scr, GFX_CONTAINER *ds);
	void (*place)       (SCREEN *scr, WIDGET *win, int x, int y, int w, int h);
	void (*remove)      (SCREEN *scr, WIDGET *win);
	void (*top)         (SCREEN *scr, WIDGET *win);
	void (*back)        (SCREEN *scr, WIDGET *win);
	void (*set_title)   (SCREEN *scr, WIDGET *win, char *title);
	void (*reorder)     (SCREEN *scr);
	void (*set_act_win) (SCREEN *scr, WIDGET *win);
};

struct screen_services {
	SCREEN *(*create) (void);

	/*
	 * When an application exists, we have to make sure
	 * that all its associated widgets get vanished from
	 * screen. The function forget_children wipes out
	 * all child widgets from all screens.
	 */
	void (*forget_children) (int app_id);
};

#define NOARG -2147483646   /* magic value to indicate the use of a default value */

#endif /* _MTK_SCREEN_H_ */
