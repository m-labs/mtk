/*
 * \brief   Interface of DOpE Entry widget module
 * \date    2004-04-07
 * \author  Norman Feske <norman.feske@genode-labs.com>
 */

/*
 * Copyright (C) 2004-2008 Norman Feske <norman.feske@genode-labs.com>
 * Genode Labs, Feske & Helmuth Systementwicklung GbR
 *
 * This file is part of the DOpE-embedded package, which is distributed
 * under the terms of the GNU General Public License version 2.
 */

#ifndef _DOPE_EDIT_H_
#define _DOPE_EDIT_H_

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


#endif /* _DOPE_EDIT_H_ */
