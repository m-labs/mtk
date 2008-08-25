/*
 * \brief   DOpE gfx 16bit image handler module
 * \date    2003-04-05
 * \author  Norman Feske <norman.feske@genode-labs.com>
 */

/*
 * Copyright (C) 2003-2008 Norman Feske <norman.feske@genode-labs.com>
 * Genode Labs, Feske & Helmuth Systementwicklung GbR
 *
 * This file is part of the DOpE-embedded package, which is distributed
 * under the terms of the GNU General Public License version 2.
 */

#include "dopestd.h"
#include "sharedmem.h"
#include "gfx_handler.h"
#include "gfx.h"

static struct sharedmem_services *shmem;

struct gfx_ds_data {
	s32 w,h;            /* width and height of the image */
	SHAREDMEM *smb;     /* shared memory block */
	u16 *pixels;        /* 16bit color values of the pixels */
};

int init_gfximg16(struct dope_services *d);

/***************************
 ** Gfx handler functions **
 ***************************/

static s32 img_get_width(struct gfx_ds_data *img)
{
	return img->w;
}

static s32 img_get_height(struct gfx_ds_data *img)
{
	return img->h;
}

static s32 img_get_type(struct gfx_ds_data *img)
{
	return GFX_IMG_TYPE_RGB16;
}

static void img_destroy(struct gfx_ds_data *img)
{
	shmem->destroy(img->smb);
	free(img);
}

static void *img_map(struct gfx_ds_data *img)
{
	return img->pixels;
}

static s32 img_share(struct gfx_ds_data *img, THREAD *dst_thread)
{
	return shmem->share(img->smb, dst_thread);
}

static s32 img_get_ident(struct gfx_ds_data *img, char *dst_ident)
{
	shmem->get_ident(img->smb, dst_ident);
	return 0;
}

/***********************
 ** Service functions **
 ***********************/


static struct gfx_ds_data *create(s32 width, s32 height, struct gfx_ds_handler **handler)
{
	struct gfx_ds_data *new;
	new = zalloc(sizeof(struct gfx_ds_data));
	if (!new) return NULL;
	new->w = width;
	new->h = height;
	new->smb = shmem->alloc(width*height*2);
	new->pixels = (u16 *)(shmem->get_address(new->smb));
	if (new->pixels) memset(new->pixels, 0, width*height*2);
	return new;
}

static s32 register_gfx_handler(struct gfx_ds_handler *handler)
{
	handler->get_width  = img_get_width;
	handler->get_height = img_get_height;
	handler->get_type   = img_get_type;
	handler->destroy    = img_destroy;
	handler->map        = img_map;
	handler->share      = img_share;
	handler->get_ident  = img_get_ident;
	return 0;
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

int init_gfximg16(struct dope_services *d)
{
	shmem = d->get_module("SharedMemory 1.0");
	d->register_module("GfxImage16 1.0",&services);
	return 1;
}
