/*
 * \brief   Keycodes used by MTK
 *
 * This file defines the keycodes used by MTK to deliver events to its
 * client applications. The definitions are derrived from Linux-2.4.25.
 */

/*
 * Copyright (C) 2004-2008 Norman Feske <norman.feske@genode-labs.com>
 * Genode Labs, Feske & Helmuth Systementwicklung GbR
 *
 * Copyright (C) 2011 Xiangfu Liu <xiangfu@sharism.cc>
 *
 * This file is part of the MTK package, which is distributed
 * under the terms of the GNU General Public License version 2.
 */

#ifndef _MTK_KEYCODES_H_
#define _MTK_KEYCODES_H_

#include "mtkasciikeycodes.h"

#define MTK_KEY_F1               (MTK_KEY_ASCII_MAX +  0)
#define MTK_KEY_F2               (MTK_KEY_ASCII_MAX +  1)
#define MTK_KEY_F3               (MTK_KEY_ASCII_MAX +  2)
#define MTK_KEY_F4               (MTK_KEY_ASCII_MAX +  3)
#define MTK_KEY_F5               (MTK_KEY_ASCII_MAX +  4)
#define MTK_KEY_F6               (MTK_KEY_ASCII_MAX +  5)
#define MTK_KEY_F7               (MTK_KEY_ASCII_MAX +  6)
#define MTK_KEY_F8               (MTK_KEY_ASCII_MAX +  7)
#define MTK_KEY_F9               (MTK_KEY_ASCII_MAX +  8)
#define MTK_KEY_F10              (MTK_KEY_ASCII_MAX +  9)
#define MTK_KEY_F11              (MTK_KEY_ASCII_MAX + 10)
#define MTK_KEY_F12              (MTK_KEY_ASCII_MAX + 11)

#define MTK_KEY_ESC              (MTK_KEY_ASCII_MAX + 12)
#define MTK_KEY_TAB              (MTK_KEY_ASCII_MAX + 13)
#define MTK_KEY_BACKSPACE        (MTK_KEY_ASCII_MAX + 14)

#define MTK_KEY_CAPSLOCK         (MTK_KEY_ASCII_MAX + 15)
#define MTK_KEY_NUMLOCK          (MTK_KEY_ASCII_MAX + 16)

#define MTK_KEY_SYSRQ            (MTK_KEY_ASCII_MAX + 17)
#define MTK_KEY_SCROLLLOCK       (MTK_KEY_ASCII_MAX + 18)
#define MTK_KEY_PAUSE            (MTK_KEY_ASCII_MAX + 19)

#define MTK_KEY_LEFTSHIFT        (MTK_KEY_ASCII_MAX + 20)
#define MTK_KEY_LEFTCTRL         (MTK_KEY_ASCII_MAX + 21)
#define MTK_KEY_LEFTALT          (MTK_KEY_ASCII_MAX + 22)
#define MTK_KEY_RIGHTSHIFT       (MTK_KEY_ASCII_MAX + 23)
#define MTK_KEY_RIGHTCTRL        (MTK_KEY_ASCII_MAX + 24)
#define MTK_KEY_RIGHTALT         (MTK_KEY_ASCII_MAX + 25)
#define MTK_KEY_LEFTMETA         (MTK_KEY_ASCII_MAX + 26)
#define MTK_KEY_RIGHTMETA        (MTK_KEY_ASCII_MAX + 27)

#define MTK_KEY_UP               (MTK_KEY_ASCII_MAX + 28)
#define MTK_KEY_LEFT             (MTK_KEY_ASCII_MAX + 29)
#define MTK_KEY_DOWN             (MTK_KEY_ASCII_MAX + 30)
#define MTK_KEY_RIGHT            (MTK_KEY_ASCII_MAX + 31)

#define MTK_KEY_PAGEUP           (MTK_KEY_ASCII_MAX + 32)
#define MTK_KEY_PAGEDOWN         (MTK_KEY_ASCII_MAX + 33)
#define MTK_KEY_HOME             (MTK_KEY_ASCII_MAX + 34)
#define MTK_KEY_END              (MTK_KEY_ASCII_MAX + 35)
#define MTK_KEY_INSERT           (MTK_KEY_ASCII_MAX + 36)
#define MTK_KEY_DELETE           (MTK_KEY_ASCII_MAX + 37)

#define MTK_BTN_LEFT             (MTK_KEY_ASCII_MAX + 38)
#define MTK_BTN_RIGHT            (MTK_KEY_ASCII_MAX + 39)
#define MTK_BTN_MIDDLE           (MTK_KEY_ASCII_MAX + 40)

#define MTK_BTN_GEAR_DOWN        (MTK_KEY_ASCII_MAX + 41)
#define MTK_BTN_GEAR_UP          (MTK_KEY_ASCII_MAX + 42)

#define MTK_KEY_MAX              (MTK_KEY_ASCII_MAX + 43)
#define MTK_KEY_INVALID          MTK_KEY_MAX

#endif /* _MTK_KEYCODES_H_ */
