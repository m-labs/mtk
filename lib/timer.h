/*
 * \brief   Interface of the timer module of MTK
 */

/*
 * Copyright (C) 2002-2008 Norman Feske <norman.feske@genode-labs.com>
 * Genode Labs, Feske & Helmuth Systementwicklung GbR
 *
 * This file is part of the MTK package, which is distributed
 * under the terms of the GNU General Public License version 2.
 */

#ifndef _MTK_TIMER_H_
#define _MTK_TIMER_H_

struct timer_services {
	u32     (*get_time) (void);
	u32     (*get_diff) (u32 time1,u32 time2);
	void    (*usleep)   (u32 num_usec);
};


#endif /* _MTK_TIMER_H_ */
