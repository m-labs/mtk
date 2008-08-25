/*
 * \brief   Interface of DOpE messenger module
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

#ifndef _DOPE_MESSENGER_H_
#define _DOPE_MESSENGER_H_

#include "event.h"

struct messenger_services {
	void    (*send_input_event) (s32 app_id,EVENT *e,char *bindarg);
	void    (*send_action_event)(s32 app_id,char *action,char *bindarg);
};

#endif /* _DOPE_MESSENGER_H_ */
