/*
 * \brief   Interface of DOpE Load Display widget module
 */

/*
 * Copyright (C) 2004-2008 Norman Feske <norman.feske@genode-labs.com>
 * Genode Labs, Feske & Helmuth Systementwicklung GbR
 *
 * This file is part of the DOpE-embedded package, which is distributed
 * under the terms of the GNU General Public License version 2.
 */

#ifndef _DOPE_LOADDISPLAY_H_
#define _DOPE_LOADDISPLAY_H_

#include "widget.h"

struct loaddisplay_methods;
struct loaddisplay_data;

#define LOADDISPLAY struct loaddisplay

#define LOADDISPLAY_VERTICAL 0x1  /* vertical loaddisplay */
#define LOADDISPLAY_FIT      0x2  /* fit bars in loaddisplay */
#define LOADDISPLAY_ABS      0x4  /* use absolute values */

struct loaddisplay {
	struct widget_methods      *gen;
	struct loaddisplay_methods *ldm;
	struct widget_data         *wd;
	struct loaddisplay_data    *ldd;
};

struct loaddisplay_methods {
	void (*set_orient) (LOADDISPLAY *ld, char *orientation);
	void (*set_from)   (LOADDISPLAY *ld, float from);
	void (*set_to)     (LOADDISPLAY *ld, float to);
	void (*barconfig)  (LOADDISPLAY *ld, char *bar_ident, char *value, char *colname);
};

struct loaddisplay_services {
	LOADDISPLAY *(*create) (void);
};


#endif /* _DOPE_LOADDISPLAY_H_ */
