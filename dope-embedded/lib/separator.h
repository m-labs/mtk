/*
 * \brief   Interface of DOpE Separator widget module
 */

/*
 * Copyright (C) 2003-2008 Norman Feske <norman.feske@genode-labs.com>
 * Genode Labs, Feske & Helmuth Systementwicklung GbR
 * Copyright (C) 2010 Sebastien Bourdeauducq <sebastien.bourdeauducq@lekernel.net>
 *
 * This file is part of the DOpE-embedded package, which is distributed
 * under the terms of the GNU General Public License version 2.
 */

#ifndef _DOPE_SEPARATOR_H_
#define _DOPE_SEPARATOR_H_

#include "widget.h"
#include "variable.h"

struct separator_methods;
struct separator_data;

#define SEPARATOR struct separator

struct separator {
	struct widget_methods *gen;
	struct separator_methods  *sep;
	struct widget_data    *wd;
	struct separator_data     *sd;
};

struct separator_methods {
	void      (*set_vertical)  (SEPARATOR *, s32 v);
	s32       (*get_vertical)  (SEPARATOR *);
};

struct separator_services {
	SEPARATOR *(*create) (void);
};


#endif /* _DOPE_SEPARATOR_H_ */
