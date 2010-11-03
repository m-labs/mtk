/*
 * \brief   MTK messenger module
 */

/*
 * Copyright (C) 2004-2008 Norman Feske <norman.feske@genode-labs.com>
 * Genode Labs, Feske & Helmuth Systementwicklung GbR
 *
 * This file is part of the MTK package, which is distributed
 * under the terms of the GNU General Public License version 2.
 */

#include "mtkstd.h"
#include "event.h"
#include "messenger.h"
#include "mtklib.h"

int init_messenger(struct mtk_services *d);


/********************************
 ** Functions for internal use **
 ********************************/

static unsigned int hex2u32(const char *s)
{
	int i;
	unsigned int result=0;
	for (i=0;i<8;i++,s++) {
		if (!(*s)) return result;
		result = result*16 + (*s & 0xf);
		if (*s > '9') result += 9;
	}
	return result;
}


/***********************
 ** Service functions **
 ***********************/

static void send_input_event(s32 app_id, EVENT *e, char *bindarg)
{
	void (*callback) (mtk_event *, void *);
	void *arg;
	mtk_event de;

	callback = (void (*)(mtk_event *,void*))hex2u32(bindarg);
	arg = (void *)hex2u32(bindarg+10);

	switch (e->type) {

	case EVENT_MOUSE_ENTER:
		de.type = 1;
		de.command.cmd = "enter";
		break;

	case EVENT_MOUSE_LEAVE:
		de.type = 1;
		de.command.cmd = "leave";
		break;

	case EVENT_MOTION:
		de.type = 2;
		de.motion.rel_x = e->rel_x;
		de.motion.rel_y = e->rel_y;
		de.motion.abs_x = e->abs_x;
		de.motion.abs_y = e->abs_y;
		break;

	case EVENT_PRESS:
		de.type = 3;
		de.press.code = e->code;
		break;

	case EVENT_RELEASE:
		de.type = 4;
		de.release.code = e->code;
		break;

	case EVENT_KEY_REPEAT:
		de.type = 5;
		de.keyrepeat.code = e->code;
		break;

	default:
		return;
	}
	callback(&de, arg);
}


static void send_action_event(s32 app_id,char *action,char *bindarg)
{
	void (*callback) (mtk_event *, void *);
	void *arg;
	mtk_event de;

	callback = (void (*)(mtk_event *,void*))hex2u32(bindarg);
	arg = (void *)hex2u32(bindarg+10);

	de.type = 1;
	de.command.cmd = action;
	callback(&de, arg);
}


/**************************************
 ** Service structure of this module **
 **************************************/

static struct messenger_services services = {
	send_input_event,
	send_action_event,
};


/************************
 ** Module entry point **
 ************************/

int init_messenger(struct mtk_services *d)
{
	d->register_module("Messenger 1.0",&services);
	return 1;
}
