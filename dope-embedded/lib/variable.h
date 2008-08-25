/*
 * \brief   Interface of DOpE Variable widget module
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

#ifndef _DOPE_VARIABLE_H_
#define _DOPE_VARIABLE_H_

#include "widget.h"

struct variable_methods;
struct variable_data;

#define VARIABLE struct variable

struct variable {
	struct widget_methods    *gen;
	struct variable_methods  *var;
	struct widget_data       *wd;
	struct variable_data     *vd;
};

struct variable_methods {
	void    (*set_string) (VARIABLE *, char *new_txt);
	char   *(*get_string) (VARIABLE *);
	void    (*connect)    (VARIABLE *, WIDGET *, void(*)(WIDGET *,VARIABLE *));
	void    (*disconnect) (VARIABLE *, WIDGET *);
};

struct variable_services {
	VARIABLE *(*create) (void);
};


#endif /* _DOPE_VARIABLE_H_ */
