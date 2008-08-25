/*
 * \brief   Interface of DOpE Frame widget module
 * \date    2002-11-13
 * \author  Norman Feske <norman.feske@genode-labs.com>
 */

/*
 * Copyright (C) 2002-2008 Norman Feske <norman.feske@genode-labs.com>
 * Genode Labs, Feske & Helmuth Systementwicklung GbR
 *
 * This file is part of the DOpE-embedded package, which is distributed
 * under the terms of the GNU General Public License version 2.
 */

#ifndef _DOPE_FRAME_H_
#define _DOPE_FRAME_H_

#include "widget.h"

struct frame_methods;
struct frame_data;

#define FRAME struct frame

struct frame {
	struct widget_methods *gen;
	struct frame_methods  *frame;
	struct widget_data    *wd;
	struct frame_data     *fd;
};

struct frame_services {
	FRAME *(*create) (void);
};


#endif /* _DOPE_FRAME_H_ */
