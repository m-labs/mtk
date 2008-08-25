/*
 * \brief   DOpE messenger module
 * \date    2002-11-13
 * \author  Norman Feske <norman.feske@genode-labs.com>
 *
 * This module enables DOpE to tell its clients about
 * events. It uses DICE to communicate.
 */

/*
 * Copyright (C) 2002-2008 Norman Feske <norman.feske@genode-labs.com>
 * Genode Labs, Feske & Helmuth Systementwicklung GbR
 *
 * This file is part of the DOpE-embedded package, which is distributed
 * under the terms of the GNU General Public License version 2.
 */

#include "dopeapp-client.h"
#include "dopestd.h"
#include "event.h"
#include "appman.h"
#include "messenger.h"

static struct appman_services *appman;
static CORBA_Environment env = dice_default_environment;

int init_messenger(struct dope_services *d);


/***********************
 ** Service functions **
 ***********************/

static void send_input_event(s32 app_id,EVENT *e,char *bindarg)
{
	dope_event_u de;
	CORBA_Object listener = appman->get_listener(app_id);

	INFO(printf("Messenger(send_input_event)\n");)
	if (!listener || !e || !bindarg) return;

	switch (e->type) {

	case EVENT_MOUSE_ENTER:
		de._d = 1;
		de._u.command.cmd = "enter";
		break;

	case EVENT_MOUSE_LEAVE:
		de._d = 1;
		de._u.command.cmd = "leave";
		break;

	case EVENT_MOTION:
		de._d = 2;
		de._u.motion.rel_x = e->rel_x;
		de._u.motion.rel_y = e->rel_y;
		de._u.motion.abs_x = e->abs_x;
		de._u.motion.abs_y = e->abs_y;
		break;

	case EVENT_PRESS:
		de._d = 3;
		de._u.press.code = e->code;
		break;

	case EVENT_RELEASE:
		de._d = 4;
		de._u.release.code = e->code;
		break;

	case EVENT_KEY_REPEAT:
		de._d = 5;
		de._u.keyrepeat.code = e->code;
		break;

	default:
		return;
	}

	INFO(printf("Messenger(send_event): event type = %d\n",(int)de._d));
	INFO(printf("Messenger(send_event): try to deliver event\n");)
	dopeapp_listener_event_call(listener, &de, bindarg, &env);
	INFO(printf("Messenger(send_event): oki\n");)
}


static void send_action_event(s32 app_id,char *action,char *bindarg)
{
	dope_event_u de;
	CORBA_Object listener = appman->get_listener(app_id);

	if (!listener || !action  || !bindarg) return;
	de._d = 1;
	de._u.command.cmd = action;
	dopeapp_listener_event_call(listener,&de,bindarg,&env);
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

int init_messenger(struct dope_services *d)
{
	appman = d->get_module("ApplicationManager 1.0");

	d->register_module("Messenger 1.0",&services);
	return 1;
}
