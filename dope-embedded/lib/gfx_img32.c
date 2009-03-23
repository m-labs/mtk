/*
 * \brief   DOpE gfx 32bit image handler module
 * \date    2003-04-07
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

typedef u32 pixel_t;

static struct sharedmem_services *shmem;

struct gfx_ds_data {
	int        w, h;    /* width and height of the image */
	SHAREDMEM *smb;     /* shared memory block */
	pixel_t   *pixels;  /* 32bit color values of the pixels */
};

int init_gfximg32(struct dope_services *d);


/***************************
 ** Gfx handler functions **
 ***************************/

static int img_get_width(struct gfx_ds_data *img)
{
	return img->w;
}

static int img_get_height(struct gfx_ds_data *img)
{
	return img->h;
}

static enum img_type img_get_type(struct gfx_ds_data *img)
{
	return GFX_IMG_TYPE_RGBA32;
}

static void img_destroy(struct gfx_ds_data *img)
{
	free(img);
}

static void *img_map(struct gfx_ds_data *img)
{
	return img->pixels;
}

static int img_share(struct gfx_ds_data *img, THREAD *dst_thread)
{
	return shmem->share(img->smb, dst_thread);
}

static int img_get_ident(struct gfx_ds_data *img, char *dst_ident)
{
	shmem->get_ident(img->smb, dst_ident);
	return 0;
}


/***********************
 ** Service functions **
 ***********************/


static struct gfx_ds_data *create(int width, int height, struct gfx_ds_handler **handler)
{
	struct gfx_ds_data *new;
	new = zalloc(sizeof(struct gfx_ds_data) + width*height*4);
	if (!new) return NULL;
	new->w = width;
	new->h = height;
	new->smb = shmem->alloc(width*height*4);
	new->pixels = (u32 *)(shmem->get_address(new->smb));
	if (new->pixels) memset(new->pixels, 0, width*height*4);
	return new;
}

static int register_gfx_handler(struct gfx_ds_handler *handler)
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

int init_gfximg32(struct dope_services *d)
{
	shmem = d->get_module("SharedMemory 1.0");
	d->register_module("GfxImage32 1.0",&services);
	return 1;
}
