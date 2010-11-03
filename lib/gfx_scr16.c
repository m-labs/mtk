/*
 * \brief  MTK gfx 16bit screen handler module
 */

/*
 * Copyright (C) 2003-2008 Norman Feske <norman.feske@genode-labs.com>
 * Genode Labs, Feske & Helmuth Systementwicklung GbR
 *
 * This file is part of the MTK package, which is distributed
 * under the terms of the GNU General Public License version 2.
 */

#include "mtkstd.h"
#include "scrdrv.h"
#include "cache.h"
#include "fontman.h"
#include "clipping.h"
#include "gfx.h"
#include "gfx_handler.h"


/***********************************************
 ** Template parameters for 'gfx_functions.h' **
 ***********************************************/

typedef u16 pixel_t;

/**
 * Convert RGBA color value to physical pixel
 */
static inline pixel_t rgba_to_pixel(unsigned int rgba) {
	return rgba_to_rgb565(rgba); }

/**
 * Blend 16bit color with specified alpha value
 */
static inline pixel_t blend(pixel_t color, int alpha)
{
	return ((((alpha >> 3) * (color & 0xf81f)) >> 5) & 0xf81f)
	      | (((alpha * (color & 0x07e0)) >> 8) & 0x7e0);
}

/**
 * Dim color by 50 percent
 */
static inline pixel_t blend_half(pixel_t color)
{
	return (color & 0xf7de)>>1;
}


/**********************
 ** Module variables **
 **********************/

static struct scrdrv_services   *scrdrv;
static struct fontman_services  *fontman;
static struct clipping_services *clip;

int init_gfxscr16(struct mtk_services *d);


/*********************************************************************
 ** Private functions, instantiated for the particular color format **
 *********************************************************************/

static enum img_type scr_get_type(struct gfx_ds_data *s)
{
	return GFX_IMG_TYPE_RGB16;
}

#include "gfx_functions.h"


/***********************
 ** Service functions **
 ***********************/

static struct gfx_ds_data *create(void *fb, int width, int height, struct gfx_ds_handler **handler)
{
	scrdrv->set_screen(fb, width, height, 16);
	scr_adr    = (pixel_t *)scrdrv->get_buf_adr();
	scr_width  = scrdrv->get_scr_width();
	scr_height = scrdrv->get_scr_height();

	if (scrdrv->get_scr_depth() == 16) scr_type = GFX_IMG_TYPE_RGB16;
	else return NULL;

	clip->set_range(0, 0, scr_width - 1, scr_height - 1);
	return (void *)1;
}


/**************************************
 ** Service structure of this module **
 **************************************/

static struct gfx_handler_services services = {
	create,
	register_gfx_handler,
};


/************************
 ** Module entry point **
 ************************/

int init_gfxscr16(struct mtk_services *d)
{
	scrdrv  = d->get_module("ScreenDriver 1.0");
	fontman = d->get_module("FontManager 1.0");
	clip    = d->get_module("Clipping 1.0");

	d->register_module("GfxScreen16 1.0", &services);
	return 1;
}
