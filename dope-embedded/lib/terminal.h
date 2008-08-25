/*
 * \brief   Interface of DOpE Terminal widget module
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

#ifndef _DOPE_TERMINAL_H_
#define _DOPE_TERMINAL_H_

#include "widget.h"

struct terminal_methods;
struct terminal_data;

#define TERMINAL struct terminal

struct terminal {
	struct widget_methods   *gen;
	struct terminal_methods *term;
	struct widget_data      *wd;
	struct terminal_data    *td;
};

struct terminal_methods {
	void (*print) (TERMINAL *,char *txt);
};

struct terminal_services {
	TERMINAL *(*create) (void);
};


#endif /* _DOPE_TERMINAL_H_ */
