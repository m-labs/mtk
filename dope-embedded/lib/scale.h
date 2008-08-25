/*
 * \brief   Interface of DOpE Scale widget module
 * \date    2003-06-11
 * \author  Norman Feske <norman.feske@genode-labs.com>
 */

/*
 * Copyright (C) 2003-2008 Norman Feske <norman.feske@genode-labs.com>
 * Genode Labs, Feske & Helmuth Systementwicklung GbR
 *
 * This file is part of the DOpE-embedded package, which is distributed
 * under the terms of the GNU General Public License version 2.
 */

#ifndef _DOPE_SCALE_H_
#define _DOPE_SCALE_H_

#include "variable.h"

struct scale_methods;
struct scale_data;

#define SCALE struct scale

#define SCALE_VER  0x1

struct scale {
	struct widget_methods *gen;
	struct scale_methods  *scale;
	struct widget_data    *wd;
	struct scale_data     *sd;
};

struct scale_services {
	SCALE *(*create) (void);
};


#endif /* _DOPE_SCALE_H_ */
