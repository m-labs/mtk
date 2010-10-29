/*
 * \brief   Interface of relaxation module
 */

/*
 * Copyright (C) 2004-2008 Norman Feske <norman.feske@genode-labs.com>
 * Genode Labs, Feske & Helmuth Systementwicklung GbR
 *
 * This file is part of the DOpE-embedded package, which is distributed
 * under the terms of the GNU General Public License version 2.
 */

#ifndef _DOPE_RELAX_H_
#define _DOPE_RELAX_H_

#define RELAX struct relax
RELAX {
	float speed;
	float dst;
	float curr;
	float accel;
};

struct relax_services {
	void (*set_duration) (RELAX *, float time);
	int  (*do_relax)     (RELAX *);
};


#endif /* _DOPE_RELAX_H_ */
