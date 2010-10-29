/*
 * \brief   Interface of MTK messenger module
 */

/*
 * Copyright (C) 2002-2008 Norman Feske <norman.feske@genode-labs.com>
 * Genode Labs, Feske & Helmuth Systementwicklung GbR
 *
 * This file is part of the MTK package, which is distributed
 * under the terms of the GNU General Public License version 2.
 */

#ifndef _MTK_MESSENGER_H_
#define _MTK_MESSENGER_H_

#include "event.h"

struct messenger_services {
	void    (*send_input_event) (s32 app_id,EVENT *e,char *bindarg);
	void    (*send_action_event)(s32 app_id,char *action,char *bindarg);
};

#endif /* _MTK_MESSENGER_H_ */
