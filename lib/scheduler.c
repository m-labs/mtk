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
#include "mtkeycodes.h"


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

int mtk_init_app(const char *appname)
{
	s32 app_id = appman->reg_app(appname);
	SCOPE *rootscope = scope->create();
	INFO(printf("mtk_init_app called\n"));
	appman->set_rootscope(app_id, rootscope);
	INFO(printf("mtk_init_app returns app_id=%d\n", (int)app_id));
	return app_id;
}

int mtk_deinit_app(int app_id)
{
	INFO(printf("Server(deinit_app): application (id=%lu) deinit requested\n", app_id);)
	screen->forget_children(app_id);
	appman->unreg_app(app_id);
	return 0;
}

int mtk_cmd(int app_id, const char *cmd)
{
	INFO(printf("app %d requests mtk_cmd \"%s\"\n", (int)app_id, cmd));
	return script->exec_command(app_id, (char *)cmd, NULL, 0);
}

int mtk_cmdf(int app_id, const char *format, ...)
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

int mtk_req(int app_id, char *dst, int dst_size, const char *cmd)
{
	int ret;

	INFO(printf("mtk_req \"%s\" requested by app_id=%lu\n", cmd, (u32)app_id);)
	ret = script->exec_command(app_id, (char *)cmd, dst, dst_size);

	return ret;
}

int mtk_reqf(int app_id, char *dst, int dst_size, const char *format, ...)
{
	va_list list;

	va_start(list, format);
	vsnprintf(cmdstr, sizeof(cmdstr), format, list);
	va_end(list);
	return mtk_req(app_id, dst, dst_size, cmdstr);
}

void mtk_bind(int app_id,const char *var, const char *event_type,
               void (*callback)(mtk_event *,void *),void *arg) {
	mtk_cmdf(app_id, "%s.bind(%s, \"%08lx, %08lx\")",
	          var, event_type, (int)callback, (int)arg);
}

void mtk_bindf(int id, const char *varfmt, const char *event_type,
                void (*callback)(mtk_event *,void *), void *arg,...) {
	static char varstr[1024];
	va_list list;

	va_start(list, arg);
	vsnprintf(varstr, 1024, varfmt, list);
	va_end(list);

	snprintf(cmdstr, 256,"%s.bind(\"%s\", \"%08x, %08x\")",
	         varstr, event_type, (int)callback, (int)arg);
	mtk_cmd(id, cmdstr);
}

static int convert_type(int t)
{
	if(t >= EVENT_TYPE_USER_BASE)
		return EVENT_USER;
	switch(t) {
		case EVENT_TYPE_MOTION: return EVENT_MOTION;
		case EVENT_TYPE_PRESS: return EVENT_PRESS;
		case EVENT_TYPE_RELEASE: return EVENT_RELEASE;
		default: return -1;
	}
}

void mtk_input(mtk_event *e, int count)
{
	EVENT internal_event[MAX_EVENTS+1];
	int i;
	static int meta_l, meta_r;
	static int up, down, left, right, btn;
	static int multiplier;

	for(i=0;i<count;i++) {
		internal_event[i].type = convert_type(e[i].type);
		if(internal_event[i].type == -1) {
			printf("Unexpected event type!\n");
			return;
		}
		switch(internal_event[i].type) {
			case EVENT_MOTION:
				internal_event[i].code = 0;
				internal_event[i].abs_x = e[i].motion.abs_x;
				internal_event[i].abs_y = e[i].motion.abs_y;
				internal_event[i].rel_x = e[i].motion.rel_x;
				internal_event[i].rel_y = e[i].motion.rel_y;
				break;
			case EVENT_PRESS:
				internal_event[i].code = e[i].press.code;
				internal_event[i].abs_x = 0;
				internal_event[i].abs_y = 0;
				internal_event[i].rel_x = 0;
				internal_event[i].rel_y = 0;
				if(e[i].press.code == MTK_KEY_LEFTMETA)
					meta_l = 1;
				if(e[i].press.code == MTK_KEY_RIGHTMETA)
					meta_r = 1;
				if(meta_l || meta_r) {
					switch(e[i].press.code) {
						case MTK_KEY_UP:
							up = 1;
							internal_event[i].type = EVENT_NULL;
							break;
						case MTK_KEY_DOWN:
							down = 1;
							internal_event[i].type = EVENT_NULL;
							break;
						case MTK_KEY_LEFT:
							left = 1;
							internal_event[i].type = EVENT_NULL;
							break;
						case MTK_KEY_RIGHT:
							right = 1;
							internal_event[i].type = EVENT_NULL;
							break;
						case MTK_KEY_ENTER:
							btn = 1;
							internal_event[i].code = MTK_BTN_LEFT;
							break;
					}
				}
				break;
			case EVENT_RELEASE:
				internal_event[i].code = e[i].release.code;
				internal_event[i].abs_x = 0;
				internal_event[i].abs_y = 0;
				internal_event[i].rel_x = 0;
				internal_event[i].rel_y = 0;
				if(e[i].release.code == MTK_KEY_LEFTMETA)
					meta_l = 0;
				if(e[i].release.code == MTK_KEY_RIGHTMETA)
					meta_r = 0;
				switch(e[i].press.code) {
					case MTK_KEY_UP:
						up = 0;
						break;
					case MTK_KEY_DOWN:
						down = 0;
						break;
					case MTK_KEY_LEFT:
						left = 0;
						break;
					case MTK_KEY_RIGHT:
						right = 0;
						break;
					case MTK_KEY_ENTER:
						if(btn) {
							btn = 0;
							internal_event[i].code = MTK_BTN_LEFT;
						}
						break;
				}
				break;
		}
	}
	if(up || down || left || right) {
		if(multiplier == 0)
			multiplier = 4;
		else if(multiplier < 80)
			multiplier++;
		internal_event[count].type = EVENT_MOTION;
		internal_event[count].abs_x = 0;
		internal_event[count].abs_y = 0;
		internal_event[count].rel_x = (multiplier >> 2)*(right - left);
		internal_event[count].rel_y = (multiplier >> 2)*(down - up);
		count++;
	} else
		multiplier = 0;
	userstate->handle(internal_event, count);
	redraw->process_pixels(config_redraw_granularity);
}

int mtk_get_keystate(int app_id, int keycode)
{
	return userstate->get_keystate(keycode);
}

char mtk_get_ascii(int app_id, int keycode)
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
