/*
 * \brief   Interface of the userstate handler module of MTK
 */

/*
 * Copyright (C) 2002-2008 Norman Feske <norman.feske@genode-labs.com>
 * Genode Labs, Feske & Helmuth Systementwicklung GbR
 *
 * This file is part of the MTK package, which is distributed
 * under the terms of the GNU General Public License version 2.
 */

#ifndef _MTK_USERSTATE_H_
#define _MTK_USERSTATE_H_

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
	int    (*get)               (void);
	void    (*handle)            (EVENT *, int);
	WIDGET *(*get_mfocus)        (void);
	void    (*set_active_window) (WINDOW *, int force);
	WIDGET *(*get_selected)      (void);
	int    (*get_mx)            (void);
	int    (*get_my)            (void);
	int    (*get_mb)            (void);
	void    (*set_pos)           (int x, int y);
	int    (*get_keystate)      (int keycode);
	char    (*get_ascii)         (int keycode);
	void    (*set_max_mx)        (int max_mx);
	void    (*set_max_my)        (int max_my);
	void    (*release_app)       (int app_id);
	void    (*release_widget)    (WIDGET *);
};


#endif /* _MTK_USERSTATE_H_ */
