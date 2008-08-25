/*
 * \brief   DOpE clipping handling module
 * \date    2002-11-13
 * \author  Norman Feske <norman.feske@genode-labs.com>
 *
 * This component handles clipping stacks. A
 * clipping area can be successively shrinked
 * to meet the needs of hierarchical widgets.
 */

/*
 * Copyright (C) 2002-2008 Norman Feske <norman.feske@genode-labs.com>
 * Genode Labs, Feske & Helmuth Systementwicklung GbR
 *
 * This file is part of the DOpE-embedded package, which is distributed
 * under the terms of the GNU General Public License version 2.
 */

#include "dopestd.h"
#include "clipping.h"

#define CLIPSTACK_SIZE 64

static long clip_x1, clip_y1, clip_x2, clip_y2;
static long cstack_x1[CLIPSTACK_SIZE];
static long cstack_y1[CLIPSTACK_SIZE];
static long cstack_x2[CLIPSTACK_SIZE];
static long cstack_y2[CLIPSTACK_SIZE];
static long csp;

int init_clipping(struct dope_services *d);


/***********************
 ** Service functions **
 ***********************/

/**
 * Request functions are often called - the reason for the buffered values
 */
static long clip_get_x1 (void) {return clip_x1;}
static long clip_get_y1 (void) {return clip_y1;}
static long clip_get_x2 (void) {return clip_x2;}
static long clip_get_y2 (void) {return clip_y2;}


/**
 * Set (shrink) global clipping values
 */
static  void clip_push(long x1, long y1, long x2, long y2)
{
	if (csp >= CLIPSTACK_SIZE - 1) return;
	
	csp++;
	clip_x1 = cstack_x1[csp] = MAX(clip_x1, x1);
	clip_y1 = cstack_y1[csp] = MAX(clip_y1, y1);
	clip_x2 = cstack_x2[csp] = MIN(clip_x2, x2);
	clip_y2 = cstack_y2[csp] = MIN(clip_y2, y2);
}


/**
 * Restore previous clipping state
 */
static void clip_pop(void)
{
	if (csp <= 0) return;

	csp--;
	clip_x1 = cstack_x1[csp];
	clip_y1 = cstack_y1[csp];
	clip_x2 = cstack_x2[csp];
	clip_y2 = cstack_y2[csp];
}


/**
 * Set global clipping values to whole screen
 */
static void clip_reset(void)
{
	csp = 0;
	clip_x1 = cstack_x1[0];
	clip_y1 = cstack_y1[0];
	clip_x2 = cstack_x2[0];
	clip_y2 = cstack_y2[0];
}


/**
 * Set clipping range (screen dimensions)
 */
static void clip_set_range(long x1, long y1, long x2, long y2)
{
	csp = 0;
	clip_x1 = cstack_x1[0] = x1;
	clip_y1 = cstack_y1[0] = y1;
	clip_x2 = cstack_x2[0] = x2;
	clip_y2 = cstack_y2[0] = y2;
}


/**************************************
 ** Service structure of this module **
 **************************************/

static struct clipping_services services = {
	clip_push,
	clip_pop,
	clip_reset,
	clip_set_range,
	clip_get_x1,
	clip_get_y1,
	clip_get_x2,
	clip_get_y2
};


/************************
 ** Module entry point **
 ************************/

int init_clipping(struct dope_services *d)
{
	d->register_module("Clipping 1.0",&services);
	return 1;
}
