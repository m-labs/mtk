/*
 * \brief   Interface of input event layer of DOpE
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

#ifndef _DOPE_INPUT_H_
#define _DOPE_INPUT_H_

#include "widget.h"
#include "event.h"

struct input_services {
	int     (*get_event)    (EVENT *e);
};


#endif /* _DOPE_INPUT_H_ */
