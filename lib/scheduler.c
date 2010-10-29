/*
 * \brief   MTK scheduling module and MTKlib interface emulation
 */

/*
 * Copyright (C) 2004-2008 Norman Feske <norman.feske@genode-labs.com>
 * Genode Labs, Feske & Helmuth Systementwicklung GbR
 *
 * This file is part of the MTK package, which is distributed
 * under the terms of the GNU General Public License version 2.
 */

/* general includes */
#include <stdarg.h>
#include <stdio.h>

/* MTK server includes */
#include "mtkstd.h"
#include "userstate.h"
#include "redraw.h"
#include "scheduler.h"
#include "appman.h"
#include "script.h"
#include "scope.h"
#include "screen.h"
#include "timer.h"

/* MTK client includes */
#include "mtklib.h"


static struct scope_services     *scope;
static struct appman_services    *appman;
static struct script_services    *script;
static struct screen_services    *screen;
static struct redraw_services    *redraw;
static struct timer_services     *timer;
static struct userstate_services *userstate;

int init_simple_scheduler(struct mtk_services *d);

extern int mtk_client_main(int argc, char **argv);

int config_redraw_granularity = 350*1000;

struct mtk_services *mtk_services;

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

/*******************************
 ** MTK client lib emulation **
 *******************************/

static char cmdstr[32768];
extern int mtk_main();


long mtk_init(void)
{
	mtk_main();
	return 0;
}


void mtk_deinit(void)
{
	INFO(printf("mtk_deinit called\n"));

}


long mtk_init_app(const char *appname)
{
	s32 app_id = appman->reg_app(appname);
	SCOPE *rootscope = scope->create();
	INFO(printf("mtk_init_app called\n"));
	appman->set_rootscope(app_id, rootscope);
	INFO(printf("mtk_init_app returns app_id=%d\n", (int)app_id));
	return app_id;
}


long mtk_deinit_app(long app_id)
{
	INFO(printf("Server(deinit_app): application (id=%lu) deinit requested\n", app_id);)
	screen->forget_children(app_id);
	appman->unreg_app(app_id);
	return 0;
}


int mtk_cmd(long app_id, const char *cmd)
{
	INFO(printf("app %d requests mtk_cmd \"%s\"\n", (int)app_id, cmd));
	return script->exec_command(app_id, (char *)cmd, NULL, 0);
}


int mtk_cmdf(long app_id, const char *format, ...)
{
	int ret;
	va_list list;

	va_start(list, format);
	vsnprintf(cmdstr, sizeof(cmdstr), format, list);
	va_end(list);
	ret = mtk_cmd(app_id, cmdstr);

	return ret;
}


int mtk_cmd_seq(int app_id, ...)
{
	int ret = 0;
	const char *cmd = NULL;
	va_list list;
	va_start(list, (const char *)app_id);

	do {
		cmd = va_arg(list, const char *);
		if (cmd)
			ret = mtk_cmd(app_id, cmd);
	} while (ret >= 0 && cmd);

	return ret;
}


int mtk_req(long app_id, char *dst, int dst_size, const char *cmd)
{
	int ret;

	INFO(printf("mtk_req \"%s\" requested by app_id=%lu\n", cmd, (u32)app_id);)
	ret = script->exec_command(app_id, (char *)cmd, dst, dst_size);

	return ret;
}


int mtk_reqf(long app_id, char *dst, int dst_size, const char *format, ...)
{
	va_list list;

	va_start(list, format);
	vsnprintf(cmdstr, sizeof(cmdstr), format, list);
	va_end(list);
	return mtk_req(app_id, dst, dst_size, cmdstr);
}


void mtk_bind(long app_id,const char *var, const char *event_type,
               void (*callback)(mtk_event *,void *),void *arg) {
	mtk_cmdf(app_id, "%s.bind(%s, \"%08lx, %08lx\")",
	          var, event_type, (long)callback, (long)arg);
}


void mtk_bindf(long id, const char *varfmt, const char *event_type,
                void (*callback)(mtk_event *,void *), void *arg,...) {
	static char varstr[1024];
	va_list list;

	va_start(list, arg);
	vsnprintf(varstr, 1024, varfmt, list);
	va_end(list);

	snprintf(cmdstr, 256,"%s.bind(\"%s\", \"%08lx, %08lx\")",
	         varstr, event_type, (long)callback, (long)arg);
	mtk_cmd(id, cmdstr);
}


void mtk_process_event(long app_id)
{
	userstate->handle();
	redraw->process_pixels(config_redraw_granularity);
}


void mtk_eventloop(long app_id)
{
	while (1) mtk_process_event(app_id);
}


int mtk_events_pending(int app_id)
{
	return 1;
}


long mtk_get_keystate(long app_id, long keycode)
{
	return userstate->get_keystate(keycode);
}


char mtk_get_ascii(long app_id, long keycode)
{
	return userstate->get_ascii(keycode);
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
	process_mainloop,
};


/************************
 ** Module entry point **
 ************************/

int init_simple_scheduler(struct mtk_services *d)
{
	appman    = (struct appman_services    *)d->get_module("ApplicationManager 1.0");
	script    = (struct script_services    *)d->get_module("Script 1.0");
	userstate = (struct userstate_services *)d->get_module("UserState 1.0");
	redraw    = (struct redraw_services    *)d->get_module("RedrawManager 1.0");
	scope     = (struct scope_services     *)d->get_module("Scope 1.0");
	screen    = (struct screen_services    *)d->get_module("Screen 1.0");
	timer     = (struct timer_services     *)d->get_module("Timer 1.0");

	mtk_services = d;

	d->register_module("Scheduler 1.0",&services);
	return 1;
}
