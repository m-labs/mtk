/*
 * \brief   DOpE input driver module for Milkymist/RTEMS
 */

/*
 * Copyright (C) 2005-2008 Norman Feske <norman.feske@genode-labs.com>
 * Genode Labs, Feske & Helmuth Systementwicklung GbR
 * Copyright (C) 2010 Sebastien Bourdeauducq <sebastien.bourdeauducq@lekernel.net>
 *
 * This file is part of the DOpE-embedded package, which is distributed
 * under the terms of the GNU General Public License version 2.
 */

#include <stdio.h>
#include <assert.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include <fcntl.h>
#include <rtems.h>
#include <bsp/milkymist_usbinput.h>

/* local includes */
#include "dopestd.h"
#include "event.h"
#include "input.h"
#include "keycodes.h"

/* translation table for scancode set 2 to scancode set 1 conversion */
static int keyb_translation_table[16][16] = {
{0xff,0x43,0x41,0x3f,0x3d,0x3b,0x3c,0x58,0x64,0x44,0x42,0x40,0x3e,0x0f,0x29,0x59},
{0x65,0x38,0x2a,0x70,0x1d,0x10,0x02,0x5a,0x66,0x71,0x2c,0x1f,0x1e,0x11,0x03,0x5b},
{0x67,0x2e,0x2d,0x20,0x12,0x05,0x04,0x5c,0x68,0x39,0x2f,0x21,0x14,0x13,0x06,0x5d},
{0x69,0x31,0x30,0x23,0x22,0x15,0x07,0x5e,0x6a,0x72,0x32,0x24,0x16,0x08,0x09,0x5f},
{0x6b,0x33,0x25,0x17,0x18,0x0b,0x0a,0x60,0x6c,0x34,0x35,0x26,0x27,0x19,0x0c,0x61},
{0x6d,0x73,0x28,0x74,0x1a,0x0d,0x62,0x6e,0x3a,0x36,0x1c,0x1b,0x75,0x2b,0x63,0x76},
{0x55,0x56,0x77,0x78,0x79,0x7a,0x0e,0x7b,0x7c,0x4f,0x7d,0x4b,0x47,0x7e,0x7f,0x6f},
{0x52,0x53,0x50,0x4c,0x4d,0x48,0x01,0x45,0x57,0x4e,0x51,0x4a,0x37,0x49,0x46,0x54},
{0x80,0x81,0x82,0x41,0x54,0x85,0x86,0x87,0x88,0x89,0x8a,0x8b,0x8c,0x8d,0x8e,0x8f},
{0x90,0x91,0x92,0x93,0x94,0x95,0x96,0x97,0x98,0x99,0x9a,0x9b,0x9c,0x9d,0x9e,0x9f},
{0xa0,0xa1,0xa2,0xa3,0xa4,0xa5,0xa6,0xa7,0xa8,0xa9,0xaa,0xab,0xac,0xad,0xae,0xaf},
{0xb0,0xb1,0xb2,0xb3,0xb4,0xb5,0xb6,0xb7,0xb8,0xb9,0xba,0xbb,0xbc,0xbd,0xbe,0xbf},
{0xc0,0xc1,0xc2,0xc3,0xc4,0xc5,0xc6,0xc7,0xc8,0xc9,0xca,0xcb,0xcc,0xcd,0xce,0xcf},
{0xd0,0xd1,0xd2,0xd3,0xd4,0xd5,0xd6,0xd7,0xd8,0xd9,0xda,0xdb,0xdc,0xdd,0xde,0xdf},
{0xe0,0xe1,0xe2,0xe3,0xe4,0xe5,0xe6,0xe7,0xe8,0xe9,0xea,0xeb,0xec,0xed,0xee,0xef},
{0xf0,0xf1,0xf2,0xf3,0xf4,0xf5,0xf6,0xf7,0xf8,0xf9,0xfa,0xfb,0xfc,0xfd,0xfe,0xff}};


int init_input(struct dope_services *d);

/**********************
 ** System interface **
 **********************/

static int input_fd;

#define MOUSE_LEFT       0x01000000
#define MOUSE_RIGHT      0x02000000
#define MOUSE_HOR_MASK   0x00ff0000
#define MOUSE_VER_MASK   0x0000ff00
#define MOUSE_HOR_SHIFT  16
#define MOUSE_VER_SHIFT  8
#define MOUSE_HOR_NEG    0x00800000
#define MOUSE_VER_NEG    0x00008000

#define KEYBD_TYPE_MASK     0x100
#define KEYBD_CODE_MASK      0xff

static inline void write_event(EVENT *e, int type, int code, int rel_x, int rel_y)
{
	e->type = type;
	e->code = code;
	e->rel_x = rel_x;
	e->rel_y = rel_y;
}

static u32 old_mstate;
static u32 new_mstate;

/**
 * Convert mouse event to dope event
 */
static int handle_mouse_event(EVENT *e)
{
	/* left mouse button pressed */
	if (!(old_mstate & MOUSE_LEFT) && (new_mstate & MOUSE_LEFT)) {
		write_event(e, EVENT_PRESS, DOPE_BTN_LEFT, 0, 0);
		old_mstate |= MOUSE_LEFT;
		return 1;
	}

	/* left mouse button released */
	if ((old_mstate & MOUSE_LEFT) && !(new_mstate & MOUSE_LEFT)) {
		write_event(e, EVENT_RELEASE, DOPE_BTN_LEFT, 0, 0);
		old_mstate &= ~MOUSE_LEFT;
		return 1;
	}

	/* right mouse button pressed */
	if (!(old_mstate & MOUSE_RIGHT) && (new_mstate & MOUSE_RIGHT)) {
		write_event(e, EVENT_PRESS, DOPE_BTN_RIGHT, 0, 0);
		old_mstate |= MOUSE_RIGHT;
		return 1;
	}

	/* right mouse button released */
	if ((old_mstate & MOUSE_RIGHT) && !(new_mstate & MOUSE_RIGHT)) {
		write_event(e, EVENT_RELEASE, DOPE_BTN_RIGHT, 0, 0);
		old_mstate &= ~MOUSE_RIGHT;
		return 1;
	}

	/* check for mouse motion */
	if ((new_mstate & MOUSE_HOR_MASK) || (new_mstate & MOUSE_VER_MASK)) {
		write_event(e, EVENT_MOTION, 0,
		      (new_mstate & MOUSE_HOR_NEG) ? (((new_mstate & MOUSE_HOR_MASK) >> MOUSE_HOR_SHIFT) | 0xFFFFFF00) : (new_mstate & MOUSE_HOR_MASK) >> MOUSE_HOR_SHIFT,
		      (new_mstate & MOUSE_VER_NEG) ?  (int)((new_mstate & MOUSE_VER_MASK) >> MOUSE_VER_SHIFT | 0xFFFFFF00) : (int)((new_mstate & MOUSE_VER_MASK) >> MOUSE_VER_SHIFT) );
		/* other events (buttons) have already been processed */
		old_mstate = new_mstate;
		return 1;
	}

	/* no events for us, update new_mstate so we read from the USB controller again */
	old_mstate = new_mstate;
	return 0;
}

/**
 * Convert keyboard event to dope event
 */
static void handle_keybd_event(EVENT *e, u32 keystate)
{
	int scancode;

	/* translate hardware scancode set 2 to software scancode set 1 */
	scancode = keyb_translation_table[(keystate>>4)&0x0F][keystate&0x0F];

	/* key released */
	if (keystate & KEYBD_TYPE_MASK)
		write_event(e, EVENT_RELEASE, scancode & KEYBD_CODE_MASK, 0, 0);

	/* key pressed */
	else write_event(e, EVENT_PRESS, scancode & KEYBD_CODE_MASK, 0, 0);
}

static int get_event(EVENT *e)
{
	if(new_mstate != old_mstate)
		return handle_mouse_event(e);
	else {
		int r;

		r = read(input_fd, &new_mstate, 4);
		if(r <= 0)
			return 0;
		else
			return handle_mouse_event(e);
	}
}

/*************************************
 * Service structure of this module **
 *************************************/

static struct input_services input = {
	get_event,
};


/************************
 ** Module entry point **
 ************************/

int init_input(struct dope_services *d)
{
	input_fd = open("/dev/usbinput", O_RDONLY);
	assert(input_fd != -1);
	ioctl(input_fd, USBINPUT_SETTIMEOUT, 1);
	old_mstate = 0;
	new_mstate = 0;
	d->register_module("Input 1.0", &input);
	return 1;
}
