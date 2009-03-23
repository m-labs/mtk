/*
 * \brief   DOpE scheduling module and DOpElib interface emulation
 * \date    2004-05-18
 * \author  Norman Feske <norman.feske@genode-labs.com>
 */

/*
 * Copyright (C) 2004-2008 Norman Feske <norman.feske@genode-labs.com>
 * Genode Labs, Feske & Helmuth Systementwicklung GbR
 *
 * This file is part of the DOpE-embedded package, which is distributed
 * under the terms of the GNU General Public License version 2.
 */

/* general includes */
#include <stdarg.h>
#include <stdio.h>

/* DOpE server includes */
#include "dopestd.h"
#include "userstate.h"
#include "redraw.h"
#include "scheduler.h"
#include "server.h"
#include "appman.h"
#include "script.h"
#include "scope.h"
#include "screen.h"
#include "timer.h"

/* DOpE client includes */
#include "dopelib.h"
#include "vscreen.h"


static struct scope_services     *scope;
static struct appman_services    *appman;
static struct script_services    *script;
static struct screen_services    *screen;
static struct redraw_services    *redraw;
static struct timer_services     *timer;
static struct userstate_services *userstate;

int init_simple_scheduler(struct dope_services *d);

extern int dope_client_main(int argc, char **argv);

int config_redraw_granularity = 150*1000;

struct dope_services *dope_services;

/***********************
 ** Service functions **
 ***********************/

/**
 * Register new real-time widget
 */
static s32 rt_add_widget(WIDGET *w, u32 period)
{
	return 0;
}


/**
 * Unregister a real-time widget
 */
static void rt_remove_widget(WIDGET *w)
{
}


/**
 * Unregister all real-time widgets of specified application
 */
static void rt_release_app(int app_id)
{
}


/**
 * Set mutex that should be unlocked after drawing operations
 */
static void rt_set_sync_mutex(WIDGET *w,MUTEX *m)
{
}


/*******************************
 ** Dope client lib emulation **
 *******************************/

static char cmdstr[1024];
extern int dope_main(int argc, char **argv);


long dope_init(void)
{
	dope_main(0, NULL);
	return 0;
}


void dope_deinit(void)
{
	INFO(printf("dope_deinit called\n"));

}


long dope_init_app(const char *appname)
{
	s32 app_id = appman->reg_app(appname);
	SCOPE *rootscope = scope->create();
	INFO(printf("dope_init_app called\n"));
	appman->set_rootscope(app_id, rootscope);
	INFO(printf("dope_init_app returns app_id=%d\n", (int)app_id));
	return app_id;
}


long dope_deinit_app(long app_id)
{
	INFO(printf("Server(deinit_app): application (id=%lu) deinit requested\n", app_id);)
	screen->forget_children(app_id);
	appman->unreg_app(app_id);
	return 0;
}


int dope_cmd(long app_id, const char *cmd)
{
	INFO(printf("app %d requests dope_cmd \"%s\"\n", (int)app_id, cmd));
	return script->exec_command(app_id, (char *)cmd, NULL, 0);
}


int dope_cmdf(long app_id, const char *format, ...)
{
	int ret;
	va_list list;

	va_start(list, format);
	vsnprintf(cmdstr, sizeof(cmdstr), format, list);
	va_end(list);
	ret = dope_cmd(app_id, cmdstr);

	return ret;
}


int dope_cmd_seq(int app_id, ...)
{
	int ret = 0;
	const char *cmd = NULL;
	va_list list;
	va_start(list, (const char *)app_id);

	do {
		cmd = va_arg(list, const char *);
		if (cmd)
			ret = dope_cmd(app_id, cmd);
	} while (ret >= 0 && cmd);

	return ret;
}


int dope_req(long app_id, char *dst, int dst_size, const char *cmd)
{
	int ret;

	INFO(printf("dope_req \"%s\" requested by app_id=%lu\n", cmd, (u32)app_id);)
	ret = script->exec_command(app_id, (char *)cmd, dst, dst_size);

	return ret;
}


int dope_reqf(long app_id, char *dst, int dst_size, const char *format, ...)
{
	va_list list;

	va_start(list, format);
	vsnprintf(cmdstr, sizeof(cmdstr), format, list);
	va_end(list);
	return dope_req(app_id, dst, dst_size, cmdstr);
}


void dope_bind(long app_id,const char *var, const char *event_type,
               void (*callback)(dope_event *,void *),void *arg) {
	dope_cmdf(app_id, "%s.bind(%s, \"%08lx, %08lx\")",
	          var, event_type, (long)callback, (long)arg);
}


void dope_bindf(long id, const char *varfmt, const char *event_type,
                void (*callback)(dope_event *,void *), void *arg,...) {
	static char varstr[1024];
	va_list list;

	va_start(list, arg);
	vsnprintf(varstr, 1024, varfmt, list);
	va_end(list);

	snprintf(cmdstr, 256,"%s.bind(\"%s\", \"%08lx, %08lx\")",
	         varstr, event_type, (long)callback, (long)arg);
	dope_cmd(id, cmdstr);
}


void dope_process_event(long app_id)
{
	userstate->handle();
	redraw->process_pixels(config_redraw_granularity);
}


void dope_eventloop(long app_id)
{
	while (1) dope_process_event(app_id);
}


int dope_events_pending(int app_id)
{
	return 1;
}


long dope_get_keystate(long app_id, long keycode)
{
	return userstate->get_keystate(keycode);
}


char dope_get_ascii(long app_id, long keycode)
{
	return userstate->get_ascii(keycode);
}


/**********************************
 ** Vscreen client lib emulation **
 **********************************/

void *vscr_get_fb(int app_id, const char *vscr_name)
{
	char retbuf[16];
	u32 res;

	if (dope_reqf(app_id, retbuf, sizeof(retbuf), "%s.map()", vscr_name) < 0) {
		INFO(printf("vscr_get_fb: could not map vscreen buffer\n"));
		return NULL;
	}
	res = strtoul(retbuf, NULL, 16);
	INFO(printf("%s map returned %s = %x\n", vscr_name, retbuf, (int)res));
	return (void *)res;
}


int vscr_free_fb(void *fb_adr)
{
	INFO(printf("vscr_free_fb called - not yet implemented\n"));
	return 0;
}


static void process_mainloop(void)
{
	return;
}


/**************************************
 ** Service structure of this module **
 **************************************/

static struct scheduler_services services = {
	rt_add_widget,
	rt_remove_widget,
	rt_release_app,
	rt_set_sync_mutex,
	process_mainloop,
};


/************************
 ** Module entry point **
 ************************/

int init_simple_scheduler(struct dope_services *d)
{
	appman    = (struct appman_services    *)d->get_module("ApplicationManager 1.0");
	script    = (struct script_services    *)d->get_module("Script 1.0");
	userstate = (struct userstate_services *)d->get_module("UserState 1.0");
	redraw    = (struct redraw_services    *)d->get_module("RedrawManager 1.0");
	scope     = (struct scope_services     *)d->get_module("Scope 1.0");
	screen    = (struct screen_services    *)d->get_module("Screen 1.0");
	timer     = (struct timer_services     *)d->get_module("Timer 1.0");

	dope_services = d;

	d->register_module("Scheduler 1.0",&services);
	return 1;
}
