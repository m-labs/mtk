/*
 * \brief   DOpE relaxation module
 */

/*
 * Copyright (C) 2004-2008 Norman Feske <norman.feske@genode-labs.com>
 * Genode Labs, Feske & Helmuth Systementwicklung GbR
 *
 * This file is part of the DOpE-embedded package, which is distributed
 * under the terms of the GNU General Public License version 2.
 */

#include "dopestd.h"
#include "relax.h"

int init_relax(struct dope_services *d);


/***********************
 ** Service functions **
 ***********************/

/**
 * Define duration of relaxation process
 */
static void set_duration(RELAX *r, float time)
{
	float delta = (r->curr > r->dst) ? r->curr - r->dst : r->dst - r->curr;
	if (time == 0) time = 0.00001;
	r->accel = 4*delta/(time*time);

	/* assure an acceleration of at least 1.0 */
	if (r->accel < 1.0) r->accel = 1.0;
}


/**
 * Do a relaxation iteration
 */
static int relax_do_relax(RELAX *r)
{
	float delta;

	/* destination value reached? */
	if (r->curr == r->dst) {
		r->speed = 0;
		return 0;
	}

	/* change value with current speed */
	if (r->curr > r->dst) {
		r->curr -= r->speed;
		if (r->curr < r->dst) r->curr = r->dst;
	} else {
		r->curr += r->speed;
		if (r->curr > r->dst) r->curr = r->dst;
	}

	/* adapt speed */
	delta = r->curr - r->dst;
	if (delta < 0) delta = -delta;

	if (r->speed * r->speed < delta * r->accel)
		r->speed += r->accel;
	else
		r->speed -= r->accel;

	if (r->speed < 1) r->speed = 1;

	return 1;  /* relaxation in progress */
};


/**************************************
 ** Service structure of this module **
 **************************************/

static struct relax_services services = {
	set_duration,
	relax_do_relax,
};


/************************
 ** Module entry point **
 ************************/

int init_relax(struct dope_services *d)
{
	d->register_module("Relax 1.0", &services);
	return 1;
}
