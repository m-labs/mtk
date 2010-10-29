/*
 * \brief   Interface of handler of gfx container type
 */

/*
 * Copyright (C) 2003-2008 Norman Feske <norman.feske@genode-labs.com>
 * Genode Labs, Feske & Helmuth Systementwicklung GbR
 *
 * This file is part of the DOpE-embedded package, which is distributed
 * under the terms of the GNU General Public License version 2.
 */

#ifndef _DOPE_GFX_HANDLER_H_
#define _DOPE_GFX_HANDLER_H_

#include "gfx.h"

struct gfx_ds_data;
struct gfx_ds_handler;

struct gfx_handler_services {
	struct gfx_ds_data *(*create) (int width, int height, struct gfx_ds_handler **handler);
	int (*register_gfx_handler) (struct gfx_ds_handler *handler);
};

struct gfx_ds {
	struct gfx_ds_handler *handler;   /* ds type dependent handler callbacks  */
	struct gfx_ds_data    *data;      /* ds type dependent data               */
	int update_cnt;                   /* update counter (used for ds caching) */
	int ref_cnt;                      /* reference counter                    */
	int cache_idx;
};

struct gfx_ds_handler {

	int           (*get_width)  (struct gfx_ds_data *ds);
	int           (*get_height) (struct gfx_ds_data *ds);
	enum img_type (*get_type)   (struct gfx_ds_data *ds);

	void  (*destroy)   (struct gfx_ds_data *ds);
	void *(*map)       (struct gfx_ds_data *ds);
	void  (*unmap)     (struct gfx_ds_data *ds);
	void  (*update)    (struct gfx_ds_data *ds, int x, int y, int w, int h);
	int   (*get_ident) (struct gfx_ds_data *ds, char *dst_ident);

	void (*draw_hline)  (struct gfx_ds_data *ds, int x, int y, int w, color_t rgba);
	void (*draw_vline)  (struct gfx_ds_data *ds, int x, int y, int h, color_t rgba);
	void (*draw_fill)   (struct gfx_ds_data *ds, int x, int y, int w, int h, color_t rgba);
	void (*draw_slice)  (struct gfx_ds_data *ds, int x,  int y,  int w,  int h,
	                                            int sx, int sy, int sw, int sh,
	                     struct gfx_ds *img, u8 alpha);
	void (*draw_img)    (struct gfx_ds_data *ds, int x,  int y,  int w,  int h,
	                     struct gfx_ds *img, u8 alpha);
	void (*draw_string) (struct gfx_ds_data *ds, int x, int y, color_t fg_rgba,
	                     color_t bg_rgba, int fnt_id, char *str);

	void (*push_clipping)  (struct gfx_ds_data *ds, int x, int y, int w, int h);
	void (*pop_clipping)   (struct gfx_ds_data *ds);
	void (*reset_clipping) (struct gfx_ds_data *ds);

	int (*get_clip_x) (struct gfx_ds_data *ds);
	int (*get_clip_y) (struct gfx_ds_data *ds);
	int (*get_clip_w) (struct gfx_ds_data *ds);
	int (*get_clip_h) (struct gfx_ds_data *ds);

	void (*set_mouse_cursor) (struct gfx_ds_data *ds, struct gfx_ds *cursor);
	void (*set_mouse_pos)    (struct gfx_ds_data *ds, int x, int y);
};

#endif /* _DOPE_GFX_HANDLER_H_ */
