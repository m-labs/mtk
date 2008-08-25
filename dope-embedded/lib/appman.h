/*
 * \brief   Interface of DOpE application manager module
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

#ifndef _DOPE_APPMAN_H_
#define _DOPE_APPMAN_H_

#include "hashtab.h"
#include "thread.h"
#include "scope.h"

struct appman_services {
	s32      (*reg_app)          (const char *app_name);
	s32      (*unreg_app)        (u32 app_id);
	void     (*set_rootscope)    (u32 app_id, SCOPE *rootscope);
	SCOPE   *(*get_rootscope)    (u32 app_id);
	void     (*reg_listener)     (s32 app_id, void *listener);
	void     (*reg_list_thread)  (s32 app_id, THREAD *);
	void    *(*get_listener)     (s32 app_id);
	char    *(*get_app_name)     (s32 app_id);
	void     (*reg_app_thread)   (s32 app_id, THREAD *app_thread);
	THREAD  *(*get_app_thread)   (s32 app_id);
	s32      (*app_id_of_thread) (THREAD *app_thread);
	s32      (*app_id_of_name)   (char *app_name);
	void     (*lock)             (s32 app_id);
	void     (*unlock)           (s32 app_id);
};


#endif /* _DOPE_APPMAN_H_ */
