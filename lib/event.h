/*
 * \brief   MTK event structure
 */

/*
 * Copyright (C) 2002-2008 Norman Feske <norman.feske@genode-labs.com>
 * Genode Labs, Feske & Helmuth Systementwicklung GbR
 *
 * This file is part of the MTK package, which is distributed
 * under the terms of the GNU General Public License version 2.
 */

#ifndef _MTK_EVENT_H_
#define _MTK_EVENT_H_

#define EVENT_PRESS         1
#define EVENT_RELEASE       2
#define EVENT_MOTION        3
#define EVENT_MOUSE_ENTER   4
#define EVENT_MOUSE_LEAVE   5
#define EVENT_KEY_REPEAT    6
#define EVENT_ABSMOTION     7
#define EVENT_ACTION        99
#define EVENT_USER          1000

#define EVENT struct event
struct event {
	long type;
	long code;          /* key/button-code                 */
	long abs_x, abs_y;  /* current absolute mouse position */
	long rel_x, rel_y;  /* relative mouse position change  */
};


#endif /* _MTK_EVENT_H_ */
