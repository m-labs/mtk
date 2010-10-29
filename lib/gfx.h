/*
 * \brief   Interface of DOpE gfx layer
 */

/*
 * Copyright (C) 2003-2008 Norman Feske <norman.feske@genode-labs.com>
 * Genode Labs, Feske & Helmuth Systementwicklung GbR
 *
 * This file is part of the DOpE-embedded package, which is distributed
 * under the terms of the GNU General Public License version 2.
 */

#ifndef _DOPE_GFX_H_
#define _DOPE_GFX_H_

enum img_type {
	GFX_IMG_TYPE_RGB16  = 1,
	GFX_IMG_TYPE_RGBA32 = 2,
	GFX_IMG_TYPE_INDEX8 = 3,
	GFX_IMG_TYPE_NATIVE = 5,
};


/**
 * Color type
 */
typedef u32 color_t;

/**
 * Accessor functions for the color type
 *
 * The 'GFX_RGBA' and 'GFX_RGB' functions are implemented as macros
 * to make them usable as array initializers.
 */
#define GFX_RGBA(r, g, b, a) (((r)<<24) | ((g)<<16) | ((b)<<8) | (a))
#define GFX_RGB(r, g, b)     (((r)<<24) | ((g)<<16) | ((b)<<8) | 255)

static inline u16 rgba_to_rgb565(unsigned long rgba) {
	return (((rgba & 0xf8000000)>>16)
	       |((rgba & 0x00fc0000)>>13)
	       |((rgba & 0x0000f800)>>11)); }

static inline color_t gfx_red  (color_t rgba) { return (rgba>>24) & 255; }
static inline color_t gfx_green(color_t rgba) { return (rgba>>16) & 255; }
static inline color_t gfx_blue (color_t rgba) { return (rgba>>8)  & 255; }
static inline color_t gfx_alpha(color_t rgba) { return (rgba)     & 255; }


/**
 * Gfx containter interface
 */
struct gfx_ds;
#define GFX_CONTAINER struct gfx_ds

struct gfx_services {

	GFX_CONTAINER *(*alloc_scr) (char *scrmode);
	GFX_CONTAINER *(*alloc_img) (int w, int h, enum img_type img_type);

	int (*load_fnt) (char *fntname);

	int           (*get_width)  (GFX_CONTAINER *);
	int           (*get_height) (GFX_CONTAINER *);
	enum img_type (*get_type)   (GFX_CONTAINER *);

	void  (*inc_ref)    (GFX_CONTAINER *);
	void  (*dec_ref)    (GFX_CONTAINER *);
	void *(*map)        (GFX_CONTAINER *);
	void  (*unmap)      (GFX_CONTAINER *);
	void  (*update)     (GFX_CONTAINER *, int x, int y, int w, int h);
	int   (*get_ident)  (GFX_CONTAINER *, char *dst);
	int   (*get_upcnt)  (GFX_CONTAINER *);

	void (*draw_hline) (GFX_CONTAINER *, int x, int y, int w, color_t rgba);
	void (*draw_vline) (GFX_CONTAINER *, int x, int y, int h, color_t rgba);
	void (*draw_box)   (GFX_CONTAINER *, int x, int y, int w, int h, color_t rgba);
	void (*draw_slice) (GFX_CONTAINER *, int x,  int y,  int w,  int h,
	                                    int ix, int iy, int iw, int ih,
	                    GFX_CONTAINER *img, u8 alpha);
	void (*draw_img)   (GFX_CONTAINER *, int x, int y, int w, int h,
	                    GFX_CONTAINER *img, u8 alpha);
	void (*draw_string)(GFX_CONTAINER *, int x, int y,
	                                    color_t fg_rgba,
	                                    color_t bg_rgba,
	                                    int fnt_id, char *str);

	void (*push_clipping)  (GFX_CONTAINER *, int x, int y, int w, int h);
	void (*pop_clipping)   (GFX_CONTAINER *);
	void (*reset_clipping) (GFX_CONTAINER *);
	int  (*get_clip_x)     (GFX_CONTAINER *);
	int  (*get_clip_y)     (GFX_CONTAINER *);
	int  (*get_clip_w)     (GFX_CONTAINER *);
	int  (*get_clip_h)     (GFX_CONTAINER *);

	void (*set_mouse_cursor) (GFX_CONTAINER *, GFX_CONTAINER *cursor);
	void (*set_mouse_pos)    (GFX_CONTAINER *, int x, int y);
};


#endif /* _DOPE_GFX_H_ */
