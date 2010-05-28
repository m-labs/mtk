/*
 * \brief   Interface of VScreen server module
 */

/*
 * Copyright (C) 2002-2008 Norman Feske <norman.feske@genode-labs.com>
 * Genode Labs, Feske & Helmuth Systementwicklung GbR
 *
 * This file is part of the DOpE-embedded package, which is distributed
 * under the terms of the GNU General Public License version 2.
 */

#ifndef _DOPE_VSCR_SERVER_H_
#define _DOPE_VSCR_SERVER_H_

#include "thread.h"
#include "vscreen.h"

struct vscr_server_services {
	int (*start) (THREAD *, VSCREEN *);
};


#endif /* _DOPE_VSCR_SERVER_H_ */
