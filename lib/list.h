/*
 * \brief   Interface of MTK Label widget module
 */

/*
 * Copyright (C) 2003-2008 Norman Feske <norman.feske@genode-labs.com>
 * Genode Labs, Feske & Helmuth Systementwicklung GbR
 *
 * This file is part of the MTK package, which is distributed
 * under the terms of the GNU General Public License version 2.
 */

#ifndef _MTK_LIST_H_
#define _MTK_LIST_H_

#include "widget.h"
#include "variable.h"

struct list_methods;
struct list_data;

#define LIST struct list

struct list {
	struct widget_methods *gen;
	struct list_methods  *lst;
	struct widget_data    *wd;
	struct list_data     *ld;
};

struct list_methods {
	void      (*set_text)       (LIST *, char *new_txt);
	char     *(*get_text)       (LIST *);
	void      (*set_font)       (LIST *, char *new_fontname);
	char     *(*get_font)       (LIST *);
	void      (*set_selection)  (LIST *, int selection);
	int       (*get_selection)  (LIST *);
};

struct list_services {
	LIST *(*create) (void);
};


#endif /* _MTK_LIST_H_ */
