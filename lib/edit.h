/*
 * \brief   Interface of MTK Entry widget module
 */

/*
 * Copyright (C) 2004-2008 Norman Feske <norman.feske@genode-labs.com>
 * Genode Labs, Feske & Helmuth Systementwicklung GbR
 * Copyright (C) 2010 Sebastien Bourdeauducq <sebastien.bourdeauducq@lekernel.net>
 *
 * This file is part of the MTK package, which is distributed
 * under the terms of the GNU General Public License version 2.
 */

#ifndef _MTK_EDIT_H_
#define _MTK_EDIT_H_

struct edit_methods;
struct edit_data;

#define EDIT struct edit

struct edit {
	struct widget_methods *gen;
	struct edit_methods  *but;
	struct widget_data    *wd;
	struct edit_data     *ed;
};

struct edit_methods {
	void    (*set_text)    (EDIT *, char *new_txt);
	char   *(*get_text)    (EDIT *);
};

struct edit_services {
	EDIT *(*create) (void);
};


#endif /* _MTK_EDIT_H_ */
