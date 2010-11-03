/*
 * \brief   Interface of keymap module
 */

/*
 * Copyright (C) 2002-2008 Norman Feske <norman.feske@genode-labs.com>
 * Genode Labs, Feske & Helmuth Systementwicklung GbR
 *
 * This file is part of the MTK package, which is distributed
 * under the terms of the GNU General Public License version 2.
 */

#ifndef _MTK_KEYMAP_H_
#define _MTK_KEYMAP_H_


#define KEYMAP_SWITCH_LSHIFT    0x01
#define KEYMAP_SWITCH_RSHIFT    0x02
#define KEYMAP_SWITCH_LCONTROL  0x04
#define KEYMAP_SWITCH_RCONTROL  0x08
#define KEYMAP_SWITCH_ALT       0x10
#define KEYMAP_SWITCH_ALTGR     0x11


struct keymap_services {
	char    (*get_ascii) (int keycode,int switches);
};

#endif /* _MTK_KEYMAP_H_ */
