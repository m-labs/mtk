/*
 * \brief   Utility macros to deal with gfx containers
 * \date    2005-08-11
 * \author  Norman Feske <norman.feske@genode-labs.com>
 */

/*
 * Copyright (C) 2005-2008 Norman Feske <norman.feske@genode-labs.com>
 * Genode Labs, Feske & Helmuth Systementwicklung GbR
 *
 * This file is part of the DOpE-embedded package, which is distributed
 * under the terms of the GNU General Public License version 2.
 */

#ifndef _DOPE_GFX_MACROS_H_
#define _DOPE_GFX_MACROS_H_

#include "gfx.h"

#define RANGE_SIZE 32

/**
 * Create range image with specified scale and offset per color channel
 */
static inline GFX_CONTAINER *gen_range_img(struct gfx_services *gfx,
                                           int r_scale,  int g_scale,  int b_scale,
                                           int r_offset, int g_offset, int b_offset) {
	GFX_CONTAINER *img;
	s32 i, j;
	u16 *dst;

	img = gfx->alloc_img(RANGE_SIZE, RANGE_SIZE, GFX_IMG_TYPE_RGB16);
	dst = gfx->map(img);

	for (j = 0; j < RANGE_SIZE; j++) {
		for (i = 0; i < RANGE_SIZE; i++) {
			int b = 3*i + 3*j;
			*(dst++) = GFX_RGB16(((r_scale*b)>>8) + r_offset,
			                     ((g_scale*b)>>8) + g_offset,
			                     ((b_scale*b)>>8) + b_offset);
		}
	}

	gfx->unmap(img);
	return img;
}


#endif /* _DOPE_GFX_MACROS_H_ */
