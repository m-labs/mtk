/*
 * \brief   DOpE dummy timer module
 * \date    2004-05-19
 * \author  Norman Feske <norman.feske@genode-labs.com>
 */

/*
 * Copyright (C) 2004-2008 Norman Feske <norman.feske@genode-labs.com>
 * Genode Labs, Feske & Helmuth Systementwicklung GbR
 *
 * This file is part of the DOpE-embedded package, which is distributed
 * under the terms of the GNU General Public License version 2.
 */

/**
 * Fpga includes
 */
#include <xparameters.h>
#include <xio.h>
#include <xtmrctr.h>

/**
 * DOpE internal includes
 */
#include "dopestd.h"
#include "timer.h"


static XTmrCtr timer_instance;
static u32     ticks_per_usec;

int init_timer(struct dope_services *d);


/***********************
 ** Service functions **
 ***********************/

/**
 * Return current system time counter in microseconds
 */
static u32 get_time(void)
{
	u32 curr_ticks = XTmrCtr_GetValue(&timer_instance, 0);
	return curr_ticks/ticks_per_usec;
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
	return time2 - time1;
}


/**
 * Wait specified number of microseconds
 */
static void wait_usec(u32 num_usec)
{
	u32 start = get_time();
	while (get_diff(start, get_time()) < num_usec);
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
	XTmrCtr_Initialize(&timer_instance, XPAR_XPS_TIMER_1_DEVICE_ID);
	XTmrCtr_Start(&timer_instance, 0);
	XTmrCtr_Reset(&timer_instance, 0);

	ticks_per_usec = XPAR_CPU_CORE_CLOCK_FREQ_HZ/(1000*1000);

	d->register_module("Timer 1.0",&services);
	return 1;
}
