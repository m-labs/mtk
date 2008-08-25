/*
 * \brief   DOpE timer module
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

#include <unistd.h>
#include "SDL/SDL.h"
#include "dopestd.h"
#include "timer.h"

int init_timer(struct dope_services *d);


/***********************
 ** Service functions **
 ***********************/

/**
 * Return current system time counter in microseconds
 */
static u32 get_time(void)
{
  return SDL_GetTicks()*1000;
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

int init_timer(struct dope_services *d)
{
	d->register_module("Timer 1.0",&services);
	return 1;
}
