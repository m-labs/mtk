/*
 * \brief   MTK Variable widget module
 */

/*
 * Copyright (C) 2002-2008 Norman Feske <norman.feske@genode-labs.com>
 * Genode Labs, Feske & Helmuth Systementwicklung GbR
 *
 * This file is part of the MTK package, which is distributed
 * under the terms of the GNU General Public License version 2.
 */

struct variable;
#define WIDGET struct variable

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "mtkstd.h"
#include "gfx.h"
#include "widget_data.h"
#include "widget_help.h"
#include "variable.h"
#include "fontman.h"
#include "script.h"
#include "widman.h"
#include "list_macros.h"

static struct widman_services  *widman;
static struct gfx_services     *gfx;
static struct fontman_services *font;
static struct script_services  *script;

#define VARIABLE_ALIGN_LEFT   0x01
#define VARIABLE_ALIGN_RIGHT  0x02
#define VARIABLE_ALIGN_TOP    0x04
#define VARIABLE_ALIGN_BOTTOM 0x08

struct variable_connection;
struct variable_connection {
	WIDGET *widget;
	void (*notify)(WIDGET *w, VARIABLE *v);
	struct variable_connection *next;
};

struct variable_data {
	char  *text;
	s16    font_id;
	s16    flags;
	struct variable_connection *connections;
};

int init_variable(struct mtk_services *d);

#define BLACK_SOLID GFX_RGBA(0, 0, 0, 255)


/****************************
 ** General widget methods **
 ****************************/

static int var_draw(VARIABLE *v, struct gfx_ds *ds, long x, long y, WIDGET *origin)
{
	int tx = x + v->wd->x + 2;
	int ty = y + v->wd->y + 2;

	if (origin == v) return 1;
	if (origin) return 0;

	if (v->vd->text) {
		gfx->draw_string(ds, tx, ty, BLACK_SOLID, 0, v->vd->font_id, v->vd->text);
	} else {
		gfx->draw_string(ds, tx, ty, BLACK_SOLID, 0, v->vd->font_id, "<no value>");
	}
	return 1;
}


/**
 * Determine min/max size of a variable widget
 */
static void var_calc_minmax(VARIABLE *v)
{
	char *txt = "<no_value>";
	if (v->vd->text) txt = v->vd->text;
	
	v->wd->min_w = v->wd->max_w = font->calc_str_width (v->vd->font_id, txt) + 4;
	v->wd->min_h = v->wd->max_h = font->calc_str_height(v->vd->font_id, txt) + 4;
}


/**
 * Free variable connection struct
 *
 * This function is only called by var_free_data.
 */
static void free_var_connection(struct variable_connection *c)
{
	if (c && c->widget) c->widget->gen->dec_ref(c->widget);
}


/**
 * Free variable widget data
 */
static void var_free_data(VARIABLE *v)
{
	FREE_CONNECTED_LIST(struct variable_connection, v->vd->connections, free_var_connection);
	if (v->vd->text) free(v->vd->text);
}


/**
 * Return widget type identifier
 */
static char *var_get_type(VARIABLE *v)
{
	return "Variable";
}


/*******************************
 ** Variable specific methods **
 *******************************/

static void var_set_value(VARIABLE *v, char *new_txt)
{
	struct variable_connection *cc;

	if (v->vd->text) free(v->vd->text);
	v->vd->text = strdup(new_txt);

	/* notify all connected widgets */
	cc = v->vd->connections;
	while (cc) {
		cc->notify(cc->widget, v);
		cc = cc->next;
	}
	v->wd->update |= WID_UPDATE_MINMAX;
}


static char *var_get_value(VARIABLE *v)
{
	if (!v->vd->text) return "<undefined>";
	return v->vd->text;
}


static void var_connect(VARIABLE *v, WIDGET *w, void (*notify)(WIDGET *w, VARIABLE *v))
{
	struct variable_connection *new;

	if (!w || !notify) return;

	w->gen->inc_ref(w);

	new = (struct variable_connection *)zalloc(sizeof(struct variable_connection));
	new->widget = w;
	new->notify = notify;
	new->next = v->vd->connections;
	v->vd->connections = new;
}


static void var_disconnect(VARIABLE *v, WIDGET *w)
{
}


static struct widget_methods gen_methods;
static struct variable_methods var_methods = {
	var_set_value,
	var_get_value,
	var_connect,
	var_disconnect,
};


/***********************
 ** Service functions **
 ***********************/

static VARIABLE *create(void)
{
	VARIABLE *new = ALLOC_WIDGET(struct variable);
	SET_WIDGET_DEFAULTS(new, struct variable, &var_methods);

	/* set variable specific attributes */
	new->vd->font_id = 1;
	var_calc_minmax(new);
	return new;
}


/**************************************
 ** Service structure of this module **
 **************************************/

static struct variable_services services = {
	create
};


/************************
 ** Module entry point **
 ************************/

static void build_script_lang(void)
{
	void *widtype;

	widtype = script->reg_widget_type("Variable", (void *(*)(void))create);
	script->reg_widget_attrib(widtype, "string value", var_get_value, var_set_value, gen_methods.update);
	widman->build_script_lang(widtype, &gen_methods);
}


int init_variable(struct mtk_services *d)
{
	widman  = d->get_module("WidgetManager 1.0");
	gfx     = d->get_module("Gfx 1.0");
	font    = d->get_module("FontManager 1.0");
	script  = d->get_module("Script 1.0");

	/* define general widget functions */
	widman->default_widget_methods(&gen_methods);

	gen_methods.get_type    = var_get_type;
	gen_methods.draw        = var_draw;
	gen_methods.calc_minmax = var_calc_minmax;
	gen_methods.free_data   = var_free_data;

	build_script_lang();

	d->register_module("Variable 1.0", &services);
	return 1;
}
