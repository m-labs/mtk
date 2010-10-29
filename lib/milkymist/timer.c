/*
 * \brief   MTK dummy timer module
 * \date    2004-05-19
 * \author  Norman Feske <norman.feske@genode-labs.com>
 */

/*
 * Copyright (C) 2004-2008 Norman Feske <norman.feske@genode-labs.com>
 * Genode Labs, Feske & Helmuth Systementwicklung GbR
 *
 * This file is part of the MTK-embedded package, which is distributed
 * under the terms of the GNU General Public License version 2.
 */

#include <rtems.h>
#include <unistd.h>

#include "mtkstd.h"
#include "timer.h"

int init_timer(struct mtk_services *d);


/***********************
 ** Service functions **
 ***********************/

#define MICROSECONDS_PER_TICK 10000

/**
 * Return current system time counter in microseconds
 */
static u32 get_time(void)
{
	return rtems_clock_get_ticks_since_boot()*MICROSECONDS_PER_TICK;
}


/**
 * Return difference between two times
 */
static u32 get_diff(u32 time1,u32 time2)
{
	/* overflow check */
	if (time1>time2) {
		time1 -= time2;
		return (u32)0xffffffff - time1;
	}
	return time2-time1;
}


/**
 * Wait specified number of microseconds
 */
static void wait_usec(u32 num_usec)
{
	usleep(num_usec);
}


/**************************************
 ** Service structure of this module **
 **************************************/

static struct timer_services services = {
	get_time,
	get_diff,
	wait_usec,
};


/************************
 ** Module entry point **
 ************************/

int init_timer(struct mtk_services *d)
{
	d->register_module("Timer 1.0", &services);
	return 1;
}
