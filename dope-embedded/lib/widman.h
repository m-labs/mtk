/*
 * \brief   Interface of the widget manager module of DOpE
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

#ifndef _DOPE_WIDMAN_H_
#define _DOPE_WIDMAN_H_

#include "widget.h"

struct widman_services {
	void (*default_widget_data)    (struct widget_data *);
	void (*default_widget_methods) (struct widget_methods *);
	void (*build_script_lang)      (void *widtype,struct widget_methods *);
};


#endif /* _DOPE_WIDMAN_H_ */
