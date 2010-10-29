/*
 * \brief   Interface of MTK Scope widget module
 */

/*
 * Copyright (C) 2003-2008 Norman Feske <norman.feske@genode-labs.com>
 * Genode Labs, Feske & Helmuth Systementwicklung GbR
 *
 * This file is part of the MTK package, which is distributed
 * under the terms of the GNU General Public License version 2.
 */

#ifndef _MTK_SCOPE_H_
#define _MTK_SCOPE_H_

#include "hashtab.h"
#include "widget.h"

struct scope_methods;
struct scope_data;

#define SCOPE struct scope

struct scope {
	struct widget_methods *gen;
	struct scope_methods  *scope;
	struct widget_data    *wd;
	struct scope_data     *sd;
};

struct scope_methods {
	int     (*set_var)      (SCOPE *s, char *type, char *name, int len, WIDGET *value);
	WIDGET *(*get_var)      (SCOPE *s, char *name, int len);
	char   *(*get_vartype)  (SCOPE *s, char *name, int len);
	SCOPE  *(*get_subscope) (SCOPE *s, char *name, int len);
};

struct scope_services {
	SCOPE *(*create) (void);
};


#endif /* _MTK_SCOPE_H_ */
