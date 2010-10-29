/*
 * \brief   Interface of input event layer of MTK
 */

/*
 * Copyright (C) 2002-2008 Norman Feske <norman.feske@genode-labs.com>
 * Genode Labs, Feske & Helmuth Systementwicklung GbR
 *
 * This file is part of the MTK package, which is distributed
 * under the terms of the GNU General Public License version 2.
 */

#ifndef _MTK_INPUT_H_
#define _MTK_INPUT_H_

#include "widget.h"
#include "event.h"

struct input_services {
	int     (*get_event)    (EVENT *e);
};


#endif /* _MTK_INPUT_H_ */
