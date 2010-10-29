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

int config_redraw_granularity = 350*1000;


/*******************************
 ** MTK client lib emulation **
 *******************************/

static char cmdstr[32768];

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

static long convert_type(long t)
{
	switch(t) {
		case EVENT_TYPE_MOTION: return EVENT_MOTION;
		case EVENT_TYPE_PRESS: return EVENT_PRESS;
		case EVENT_TYPE_RELEASE: return EVENT_RELEASE;
		default: return -1;
	}
}

void mtk_input(mtk_event *e)
{
	if(e == NULL)
		userstate->handle(NULL);
	else {
		EVENT internal_event;

		internal_event.type = convert_type(e->type);
		if(internal_event.type == -1) {
			userstate->handle(NULL);
			return;
		}
		switch(internal_event.type) {
			case EVENT_TYPE_MOTION:
				internal_event.code = 0;
				internal_event.abs_x = e->motion.abs_x;
				internal_event.abs_y = e->motion.abs_y;
				internal_event.rel_x = e->motion.rel_x;
				internal_event.rel_y = e->motion.rel_y;
				break;
			case EVENT_TYPE_PRESS:
			case EVENT_TYPE_RELEASE:
				internal_event.code = e->press.code;
				internal_event.abs_x = 0;
				internal_event.abs_y = 0;
				internal_event.rel_x = 0;
				internal_event.rel_y = 0;
				break;
		}
		userstate->handle(&internal_event);
	}
	redraw->process_pixels(config_redraw_granularity);
}

long mtk_get_keystate(long app_id, long keycode)
{
	return userstate->get_keystate(keycode);
}

char mtk_get_ascii(long app_id, long keycode)
{
	return userstate->get_ascii(keycode);
}

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

	return 1;
}
