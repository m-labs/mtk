/*
 * Copyright (C) 2010 Sebastien Bourdeauducq <sebastien@milkymist.org>
 *
 * This file is part of the MTK package, which is distributed
 * under the terms of the GNU General Public License version 2.
 */

struct pixmap;
#define WIDGET struct pixmap

#include <stdio.h>
#include <string.h>
#include "mtkstd.h"
#include "widget_data.h"
#include "widget_help.h"
#include "gfx.h"
#include "redraw.h"
#include "pixmap.h"
#include "script.h"
#include "widman.h"
#include "messenger.h"
#include "keycodes.h"
#include "window.h"

static struct widman_services      *widman;
static struct script_services      *script;
static struct redraw_services      *redraw;
static struct gfx_services         *gfx;

struct pixmap_data {
	s32 xres, yres;
	void *fb;
};

int init_pixmap(struct mtk_services *d);

/****************************
 ** General widget methods **
 ****************************/

/**
 * Draw pixmap widget
 */
static int pixm_draw(PIXMAP *pm, struct gfx_ds *ds, int x, int y, WIDGET *origin)
{
	GFX_CONTAINER *image;
	void *newb;

	x += pm->wd->x;
	y += pm->wd->y;

	if(origin == pm) return 1;
	if(origin) return 0;

	if((pm->pd->xres == 0) || (pm->pd->yres == 0) || (pm->pd->fb == NULL))
		return 1;

	image = gfx->alloc_img(pm->pd->xres, pm->pd->yres, GFX_IMG_TYPE_RGB16);
	if(image == NULL)
		return 1;
	newb = gfx->map(image);
	memcpy(newb, pm->pd->fb, pm->pd->xres*pm->pd->yres*2);
	gfx->unmap(image);
	
	gfx->push_clipping(ds, x, y, pm->wd->w, pm->wd->h);
	gfx->draw_img(ds, x, y, pm->pd->xres, pm->pd->yres, image, 255);
	gfx->pop_clipping(ds);

	gfx->dec_ref(image);

	return 1;
}

/**
 * Return widget type identifier
 */
static char *pixm_get_type(PIXMAP *pm)
{
	return "Pixmap";
}


/******************************
 ** Pixmap specific methods **
 ******************************/

static void pixm_set_w(PIXMAP *pm, int new_w)
{
	pm->pd->xres = pm->wd->min_w = pm->wd->max_w = new_w;
	pm->wd->update |= WID_UPDATE_MINMAX;
}

static int pixm_get_w(PIXMAP *pm)
{
	return pm->pd->xres;
}

static void pixm_set_h(PIXMAP *pm, int new_h)
{
	pm->pd->yres = pm->wd->min_h = pm->wd->max_h = new_h;
	pm->wd->update |= WID_UPDATE_MINMAX;
}

static int pixm_get_h(PIXMAP *pm)
{
	return pm->pd->yres;
}

static void pixm_set_fb(PIXMAP *pm, int new_fb)
{
	pm->pd->fb = (void *)new_fb;
	pm->wd->update |= WID_UPDATE_MINMAX;
}

static int pixm_get_fb(PIXMAP *pm)
{
	return (int)pm->pd->fb;
}

static void pixm_refresh(PIXMAP *pm)
{
	redraw->draw_widgetarea(pm, 0, 0, pm->pd->xres, pm->pd->yres);
}

static struct widget_methods gen_methods;
static struct pixmap_methods pixmap_methods = {
	pixm_refresh
};


/***********************
 ** Service functions **
 ***********************/

static PIXMAP *create(void)
{
	PIXMAP *new = ALLOC_WIDGET(struct pixmap);
	SET_WIDGET_DEFAULTS(new, struct pixmap, &pixmap_methods);
	new->pd->xres = 0;
	new->pd->yres = 0;
	new->pd->fb = NULL;
	return new;
}


/**************************************
 ** Service structure of this module **
 **************************************/

static struct pixmap_services services = {
	create
};


/************************
 ** Module entry point **
 ************************/

static void build_script_lang(void)
{
	void *widtype;

	widtype = script->reg_widget_type("Pixmap", (void *(*)(void))create);

	script->reg_widget_method(widtype, "void refresh()", pixm_refresh);
	script->reg_widget_attrib(widtype, "int w", pixm_get_w, pixm_set_w, gen_methods.update);
	script->reg_widget_attrib(widtype, "int h", pixm_get_h, pixm_set_h, gen_methods.update);
	script->reg_widget_attrib(widtype, "int fb", pixm_get_fb, pixm_set_fb, gen_methods.update);

	widman->build_script_lang(widtype, &gen_methods);
}


int init_pixmap(struct mtk_services *d)
{
	gfx         = d->get_module("Gfx 1.0");
	script      = d->get_module("Script 1.0");
	widman      = d->get_module("WidgetManager 1.0");
	redraw      = d->get_module("RedrawManager 1.0");

	/* define general widget functions */
	widman->default_widget_methods(&gen_methods);

	gen_methods.get_type     = pixm_get_type;
	gen_methods.draw         = pixm_draw;

	build_script_lang();

	d->register_module("Pixmap 1.0", &services);
	return 1;
}
