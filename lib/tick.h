/*
 * \brief   Interface of tick module
 */

/*
 * Copyright (C) 2004-2008 Norman Feske <norman.feske@genode-labs.com>
 * Genode Labs, Feske & Helmuth Systementwicklung GbR
 *
 * This file is part of the DOpE-embedded package, which is distributed
 * under the terms of the GNU General Public License version 2.
 */

#ifndef _DOPE_TICK_H_
#define _DOPE_TICK_H_

struct tick_services {
	int   (*add)    (s32 msec, int (*callback)(void *), void *arg);
	void  (*handle) (void);
};

#endif /* _DOPE_TICK_H_ */
