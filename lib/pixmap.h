/*
 * Copyright (C) 2010 Sebastien Bourdeauducq <sebastien@milkymist.org>
 *
 * This file is part of the MTK package, which is distributed
 * under the terms of the GNU General Public License version 2.
 */

#ifndef _MTK_PIXMAP_H_
#define _MTK_PIXMAP_H_

#include "widget.h"

struct pixmap_methods;
struct pixmap_data;

#define PIXMAP struct pixmap

#include "gfx.h"

struct pixmap {
	struct widget_methods  *gen;
	struct pixmap_methods  *pixm;
	struct widget_data     *wd;
	struct pixmap_data     *pd;
};

struct pixmap_methods {
	void (*refresh)    (PIXMAP *);
};

struct pixmap_services {
	PIXMAP *(*create) (void);
};

#endif /* _MTK_PIXMAP_H_ */
