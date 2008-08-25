/*
 * \brief   DOpE module structure
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

#ifndef _DOPE_MODULE_H_
#define _DOPE_MODULE_H_

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


#endif /* _DOPE_MODULE_H_ */
