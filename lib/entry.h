/*
 * \brief   Interface of MTK Entry widget module
 */

/*
 * Copyright (C) 2004-2008 Norman Feske <norman.feske@genode-labs.com>
 * Genode Labs, Feske & Helmuth Systementwicklung GbR
 *
 * This file is part of the MTK package, which is distributed
 * under the terms of the GNU General Public License version 2.
 */

#ifndef _MTK_ENTRY_H_
#define _MTK_ENTRY_H_

struct entry_methods;
struct entry_data;

#define ENTRY struct entry

struct entry {
	struct widget_methods *gen;
	struct entry_methods  *but;
	struct widget_data    *wd;
	struct entry_data     *ed;
};

struct entry_methods {
	void    (*set_text)    (ENTRY *, char *new_txt);
	char   *(*get_text)    (ENTRY *);
};

struct entry_services {
	ENTRY *(*create) (void);
};


#endif /* _MTK_ENTRY_H_ */
