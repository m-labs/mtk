/*
 * \brief   Interface of DOpE VTextScreen widget module
 */

/*
 * Copyright (C) 2004-2008 Norman Feske <norman.feske@genode-labs.com>
 * Genode Labs, Feske & Helmuth Systementwicklung GbR
 *
 * This file is part of the DOpE-embedded package, which is distributed
 * under the terms of the GNU General Public License version 2.
 */

#ifndef _DOPE_VTEXTSCREEN_H_
#define _DOPE_VTEXTSCREEN_H_

#include "widget.h"

struct vtextscreen_methods;
struct vtextscreen_data;

#define VTEXTSCREEN struct vtextscreen

#include "gfx.h"

struct vtextscreen {
	struct widget_methods      *gen;
	struct vtextscreen_methods *vts;
	struct widget_data         *wd;
	struct vtextscreen_data    *vd;
};

struct vtextscreen_methods {
	s32   (*probe_mode)    (VTEXTSCREEN *, s32 width, s32 height, char *mode);
	s32   (*set_mode)      (VTEXTSCREEN *, s32 width, s32 height, char *mode);
	char *(*map)           (VTEXTSCREEN *, char *dst_thread_ident);
	void  (*refresh)       (VTEXTSCREEN *, s32 x, s32 y, s32 w, s32 h);
	GFX_CONTAINER *(*get_image) (VTEXTSCREEN *);
};

struct vtextscreen_services {
	VTEXTSCREEN *(*create) (void);
};

#endif /* _DOPE_VTEXTSCREEN_H_ */
