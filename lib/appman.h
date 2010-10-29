/*
 * \brief   Interface of DOpE application manager module
 */

/*
 * Copyright (C) 2002-2008 Norman Feske <norman.feske@genode-labs.com>
 * Genode Labs, Feske & Helmuth Systementwicklung GbR
 *
 * This file is part of the DOpE-embedded package, which is distributed
 * under the terms of the GNU General Public License version 2.
 */

#ifndef _DOPE_APPMAN_H_
#define _DOPE_APPMAN_H_

#include "hashtab.h"
#include "scope.h"

struct appman_services {
	s32      (*reg_app)          (const char *app_name);
	s32      (*unreg_app)        (u32 app_id);
	void     (*set_rootscope)    (u32 app_id, SCOPE *rootscope);
	SCOPE   *(*get_rootscope)    (u32 app_id);
	char    *(*get_app_name)     (s32 app_id);
	s32      (*app_id_of_name)   (char *app_name);
};


#endif /* _DOPE_APPMAN_H_ */
