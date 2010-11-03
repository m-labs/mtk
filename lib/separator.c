/*
 * \brief   MTK Separator widget module
 */

/*
 * Copyright (C) 2002-2008 Norman Feske <norman.feske@genode-labs.com>
 * Genode Labs, Feske & Helmuth Systementwicklung GbR
 * Copyright (C) 2010 Sebastien Bourdeauducq <sebastien.bourdeauducq@lekernel.net>
 *
 * This file is part of the MTK package, which is distributed
 * under the terms of the GNU General Public License version 2.
 */

struct separator;
#define WIDGET struct separator

#include <stdio.h>
#include "mtkstd.h"
#include "gfx.h"
#include "widget_data.h"
#include "widget_help.h"
#include "variable.h"
#include "separator.h"
#include "fontman.h"
#include "script.h"
#include "widman.h"

static struct widman_services  *widman;
static struct gfx_services     *gfx;
static struct fontman_services *font;
static struct script_services  *script;

struct separator_data {
	s32       vertical;
};

int init_separator(struct mtk_services *d);


static const color_t BLACK_SOLID = GFX_RGBA(0, 0, 0, 255);
static const color_t BLACK_MIXED = GFX_RGBA(0, 0, 0, 127);
static const color_t WHITE_SOLID = GFX_RGBA(255, 255, 255, 255);
static const color_t WHITE_MIXED = GFX_RGBA(255, 255, 255, 127);
static const color_t DARK_GREY   = GFX_RGBA(80, 80, 80, 255);

/****************************
 ** General widget methods **
 ****************************/

static inline void draw_pressed_frame(GFX_CONTAINER *d, s32 x, s32 y, s32 w, s32 h)
{
	/* outer frame */
	gfx->draw_hline(d, x, y, w, BLACK_SOLID);
	gfx->draw_vline(d, x, y, h, BLACK_SOLID);
	gfx->draw_hline(d, x, y + h - 1, w, WHITE_MIXED);
	gfx->draw_vline(d, x + w - 1, y, h, WHITE_MIXED);

	/* inner frame */
	gfx->draw_hline(d, x + 1, y + 1, w - 2, BLACK_SOLID);
	gfx->draw_vline(d, x + 1, y + 1, h - 2, BLACK_SOLID);
	gfx->draw_hline(d, x + 1, y + h - 2, w - 2, WHITE_SOLID);
	gfx->draw_vline(d, x + w - 2, y + 1, h - 2, WHITE_SOLID);
}

static int sep_draw(SEPARATOR *s, struct gfx_ds *ds, int x, int y, WIDGET *origin)
{
	int w, h;
	
	if (origin == s) return 1;
	if (origin) return 0;

	x += s->wd->x;
	y += s->wd->y;
	w = s->wd->w;
	h = s->wd->h;

	gfx->push_clipping(ds, x, y, w, h);

	x += 2;
	y += 2;
	h -= 2*2;
	w -= 2*2;
	if(s->sd->vertical)
		draw_pressed_frame(ds, x, y, 3, h);
	else
		draw_pressed_frame(ds, x, y, w, 3);
	
	gfx->pop_clipping(ds);

	return 1;
}


/**
 * Determine min/max size of a separator widget
 */
static void sep_calc_minmax(SEPARATOR *s)
{
	if (s->sd->vertical) {
		s->wd->min_w = s->wd->max_w = 3 + 2 * 2;
		s->wd->min_h = 2 * 2;
		s->wd->max_h = 99999;
	} else {
		s->wd->min_w = 2 * 2;
		s->wd->max_w = 99999;
		s->wd->min_h = s->wd->max_h = 3 + 2 * 2;
	}
}

/**
 * Return widget type identifier
 */
static char *sep_get_type(SEPARATOR *s)
{
	return "Separator";
}


/****************************
 ** Separator specific methods **
 ****************************/

static void sep_set_vertical(SEPARATOR *s, s32 v)
{
	if ((!s) || (!s->sd)) return;
	s->sd->vertical = v;
	s->wd->update |= WID_UPDATE_MINMAX;
}


static s32 sep_get_vertical(SEPARATOR *s)
{
	return s->sd->vertical;
}


static struct widget_methods gen_methods;
static struct separator_methods sep_methods = {
	sep_set_vertical,
	sep_get_vertical,
};


/***********************
 ** Service functions **
 ***********************/

static SEPARATOR *create(void)
{
	SEPARATOR *new = ALLOC_WIDGET(struct separator);
	SET_WIDGET_DEFAULTS(new, struct separator, &sep_methods);

	/* set separator specific attributes */
	new->sd->vertical = 0;
	gen_methods.update(new);

	return new;
}


/**************************************
 ** Service structure of this module **
 **************************************/

static struct separator_services services = {
	create
};


/************************
 ** Module entry point **
 ************************/

static void build_script_lang(void)
{
	void *widtype;

	widtype = script->reg_widget_type("Separator", (void *(*)(void))create);

	script->reg_widget_attrib(widtype, "boolean vertical", sep_get_vertical, sep_set_vertical, gen_methods.update);

	widman->build_script_lang(widtype, &gen_methods);
}


int init_separator(struct mtk_services *d)
{
	widman  = d->get_module("WidgetManager 1.0");
	gfx     = d->get_module("Gfx 1.0");
	font    = d->get_module("FontManager 1.0");
	script  = d->get_module("Script 1.0");

	/* define general widget functions */
	widman->default_widget_methods(&gen_methods);

	gen_methods.draw        = sep_draw;
	gen_methods.get_type    = sep_get_type;
	gen_methods.calc_minmax = sep_calc_minmax;

	build_script_lang();

	d->register_module("Separator 1.0", &services);
	return 1;
}
