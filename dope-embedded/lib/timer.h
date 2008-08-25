/*
 * \brief   Interface of the timer module of DOpE
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

#ifndef _DOPE_TIMER_H_
#define _DOPE_TIMER_H_

struct timer_services {
	u32     (*get_time) (void);
	u32     (*get_diff) (u32 time1,u32 time2);
	void    (*usleep)   (u32 num_usec);
};


#endif /* _DOPE_TIMER_H_ */
