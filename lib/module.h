/*
 * \brief   MTK module structure
 */

/*
 * Copyright (C) 2002-2008 Norman Feske <norman.feske@genode-labs.com>
 * Genode Labs, Feske & Helmuth Systementwicklung GbR
 *
 * This file is part of the MTK package, which is distributed
 * under the terms of the GNU General Public License version 2.
 */

#ifndef _MTK_MODULE_H_
#define _MTK_MODULE_H_

struct module_info {
	char    *type;      /* kind of module */
	char    *name;      /* name and version */
	char    *conflict;  /* lowest version to which the module is compatibe with */
	char    *uses;      /* required modules */
};

struct module {
	struct module_info  *info;
	int                (*init)   (void);
	int                (*deinit) (void);
	void                *services;
};


#endif /* _MTK_MODULE_H_ */
