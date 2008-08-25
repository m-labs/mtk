/*
 * \brief   Interface of the userstate handler module of DOpE
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

#ifndef _DOPE_USERSTATE_H_
#define _DOPE_USERSTATE_H_

#include "widget.h"
#include "window.h"

#define USERSTATE_IDLE       0
#define USERSTATE_KEYREPEAT  1
#define USERSTATE_TOUCH      2
#define USERSTATE_DRAG       3
#define USERSTATE_GRAB       4

struct userstate_services {

	void    (*idle)    (void);
	void    (*touch)   (WIDGET *, void (*tick)   (WIDGET *, int dx, int dy),
	                              void (*release)(WIDGET *, int dx, int dy));
	void    (*drag)    (WIDGET *, void (*motion) (WIDGET *, int dx, int dy),
	                              void (*tick)   (WIDGET *, int dx, int dy),
	                              void (*release)(WIDGET *, int dx, int dy));
	void    (*grab)    (WIDGET *, void (*tick)   (WIDGET *, int dx, int dy));
	long    (*get)               (void);
	void    (*handle)            (void);
	WIDGET *(*get_mfocus)        (void);
	void    (*set_active_window) (WINDOW *, int force);
	WIDGET *(*get_selected)      (void);
	long    (*get_mx)            (void);
	long    (*get_my)            (void);
	long    (*get_mb)            (void);
	void    (*set_pos)           (long x,long y);
	long    (*get_keystate)      (long keycode);
	char    (*get_ascii)         (long keycode);
	void    (*set_max_mx)        (long max_mx);
	void    (*set_max_my)        (long max_my);
	void    (*release_app)       (int app_id);
	void    (*release_widget)    (WIDGET *);
};


#endif /* _DOPE_USERSTATE_H_ */
