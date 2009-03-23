/*
 * \brief  Generic implementation of gfx container functions
 * \author Norman Feske
 * \date   2009-03-09
 *
 * This file contains graphics functions that operate on a 'pixel_t' type.
 * They contain the generic program logic of the graphical primitives
 * independent of the actual pixel format. To use the functions, you
 * have to provide the following types and functions before including
 * this file:
 *
 * :pixel_t:    representation of a physical pixel
 * :blend:      function that blends a pixel_t according to an alpha value
 * :blend_half: function to reduce the brightness of a pixel by 50%
 * :scrdrv:     pointer to screen-driver service structure
 *
 * For an example of how to use this file, please refer to the 'gfx_scr16.c'.
 */

#ifndef _DOPE_GFX_FUNCTIONS_H_
#define _DOPE_GFX_FUNCTIONS_H_

/**
 * Variables to be initialized before using the gfx functions
 */
static int      clip_x1, clip_y1, clip_x2, clip_y2;
static pixel_t *scr_adr;
static int      scr_width, scr_height, scr_type;


/**
 * Draw a solid horizontal line
 */
static inline void solid_hline(pixel_t *dst, int width, pixel_t col)
{
	for (; width--; dst++) *dst = col;
}


/**
 * Draw a solid vertical line
 */
static inline void solid_vline(pixel_t *dst, int height, int scr_w, pixel_t col)
{
	for (; height--; dst += scr_w) *dst = col;
}


/**
 * Draw a transparent horizontal line
 */
static inline void mixed_hline(pixel_t *dst, int width, pixel_t mixcol)
{
	mixcol = blend_half(mixcol);
	for (; width--; dst++) *dst = blend_half(*dst) + mixcol;
}


/**
 * Draw a transparent vertical line in 16bit color mode
 */
static inline void mixed_vline(u16 *dst, int height, int scr_w, u16 mixcol)
{
	mixcol = blend_half(mixcol);
	for (; height--; dst += scr_w) *dst = blend_half(*dst) + mixcol;
}


/**
 * Clip image against clipping area
 */
static inline int clip_img(int clip_x1, int clip_y1, int clip_x2, int clip_y2,
                           int *x,  int *y,  int *w, int *h,
                           int *sx, int *sy, int mx, int my)
{
	/* left clipping */
	if (*x   <  clip_x1) {
		*w  -=  clip_x1 - *x;
		*sx += (clip_x1 - *x) * mx;
		*x   =  clip_x1;
	}

	/* right clipping */
	if (*w > clip_x2 - *x + 1)
		*w = clip_x2 - *x + 1;

	/* top clipping */
	if (*y   <  clip_y1) {
		*h  -=  clip_y1 - *y;
		*sy += (clip_y1 - *y) * my;
		*y   =  clip_y1;
	}

	/* bottom clipping */
	if (*h > clip_y2 - *y + 1)
		*h = clip_y2 - *y + 1;

	return ((*w >= 0) && (*h >= 0));
}


/**
 * Draw clipped image to screen
 *
 * Image and screen have the same pixel format.
 */
static inline void paint_img(int x, int y, int img_w, int img_h, pixel_t *src)
{
	int      i, j;
	int      w = img_w, h = img_h;
	pixel_t *dst, *s, *d;
	int      sx = 0, sy = 0;

	if (!clip_img(clip_x1, clip_y1, clip_x2, clip_y2,
	              &x, &y, &w, &h, &sx, &sy, 1, 1)) return;

	/* calculate start address */
	src += img_w*sy + sx;
	dst  = scr_adr + y*scr_width + x;

	/* paint... */
	for (j = h; j--; ) {

		/* copy line from image to screen */
		for (i = w, s = src, d = dst; i--; *(d++) = *(s++));
		src += img_w;
		dst += scr_width;
	}
}


static int scale_xbuf[2000];

/**
 * Draw scaled and clipped image to screen
 *
 * Image and screen have the same pixel format.
 */
static void paint_scaled_img(int x, int y, int w, int h,
                             int linewidth, int sw, int sh, u16 *src)
{
	int      mx, my;
	int      i, j;
	int      sx = 0, sy = 0;
	pixel_t *dst, *s, *d;

	/* sanity check */
	if (!src) return;

	/* use shortcut for non-scaled images */
	if ((w == sw) && (h == sh)) {
		paint_img(x, y, sw, sh, src);
		return;
	}

	mx = w ? ((long)sw<<16) / w : 0;
	my = h ? ((long)sh<<16) / h : 0;

	if (!clip_img(clip_x1, clip_y1, clip_x2, clip_y2,
	              &x, &y, &w, &h, &sx, &sy, mx, my)) return;

	/* calculate start address */
	dst = scr_adr + y*scr_width + x;

	/* calculate x offsets */
	for (i = w; i--; sx += mx)
		scale_xbuf[i] = sx >> 16;

	/* draw scaled image */
	for (j = h; j--; sy += my, dst += scr_width) {
		s = src + ((sy>>16)*linewidth);
		d = dst;
		for (i = w; i--; )
			*(d++) = *(s + scale_xbuf[i]);
	}
}


/**
 * Draw scaled and clipped 32bit argb image to screen
 */
static void paint_scaled_img_rgba32(int x, int y, int w, int h,
                                    int linewidth, int sw, int sh, u32 *src)
{
	int      mx, my;
	int      i, j;
	int      sx = 0, sy = 0;
	pixel_t *dst, *d;
	u32     *s;

	/* sanity check */
	if (!src) return;

	mx = w ? ((long)sw<<16) / w : 0;
	my = h ? ((long)sh<<16) / h : 0;

	if (!clip_img(clip_x1, clip_y1, clip_x2, clip_y2,
	              &x, &y, &w, &h, &sx, &sy, mx, my)) return;

	/* calculate start address */
	dst = scr_adr + y*scr_width + x;

	/* calculate x offsets */
	for (i = w; i--; sx += mx)
		scale_xbuf[i] = sx >> 16;

	/* draw scaled image */
	for (j = h; j--; sy += my, dst += scr_width) {
		s = src + ((sy>>16)*linewidth);
		d = dst;
		for (i = w; i--; d++) {
			u32 color = *(s + scale_xbuf[i]);
			int alpha = gfx_alpha(color);
			if (alpha) *d = blend(*d, 255 - alpha) + blend(rgba_to_pixel(color), alpha);
		}
	}
}


/***************************
 ** Gfx handler functions **
 ***************************/

static int scr_get_width(struct gfx_ds_data *s)
{
	return scr_width;
}


static int scr_get_height(struct gfx_ds_data *s)
{
	return scr_height;
}


static void scr_destroy(struct gfx_ds_data *s)
{
	scrdrv->restore_screen();
}


static void *scr_map(struct gfx_ds_data *s)
{
	return scr_adr;
}


static void scr_update(struct gfx_ds_data *s, int x, int y, int w, int h)
{
	scrdrv->update_area(x, y, x + w - 1, y + h - 1);
}


static void scr_draw_hline(struct gfx_ds_data *s, int x, int y, int w, color_t rgba)
{
	int beg_x, end_x;

	if (clip_y1 > y || clip_y2 < y) return;

	beg_x = MAX(x, clip_x1);
	end_x = MIN(x + w - 1, clip_x2);

	if (beg_x > end_x) return;

	if (gfx_alpha(rgba) > 127) {
		solid_hline(scr_adr + y*scr_width + beg_x, end_x - beg_x + 1, rgba_to_pixel(rgba));
	} else {
		mixed_hline(scr_adr + y*scr_width + beg_x, end_x - beg_x + 1, rgba_to_pixel(rgba));
	}
}


static void scr_draw_vline(struct gfx_ds_data *s, int x, int y, int h, color_t rgba)
{
	int beg_y, end_y;

	if (clip_x1 > x || clip_x2 < x) return;

	beg_y = MAX(y, clip_y1);
	end_y = MIN(y + h - 1, clip_y2);

	if (beg_y > end_y) return;

	if (gfx_alpha(rgba) > 127)
		solid_vline(scr_adr + beg_y*scr_width + x, end_y - beg_y + 1, scr_width, rgba_to_pixel(rgba));
	else
		mixed_vline(scr_adr + beg_y*scr_width + x, end_y - beg_y + 1, scr_width, rgba_to_pixel(rgba));
}


static void scr_draw_fill(struct gfx_ds_data *s, int x1, int y1, int w, int h, color_t rgba)
{
	int      x, y;
	pixel_t *dst, *dst_line;
	pixel_t  color;
	int      alpha;
	int      x2 = x1 + w - 1;
	int      y2 = y1 + h - 1;

	/* check clipping */
	if (x1 < clip_x1) x1 = clip_x1;
	if (y1 < clip_y1) y1 = clip_y1;
	if (x2 > clip_x2) x2 = clip_x2;
	if (y2 > clip_y2) y2 = clip_y2;

	if ((x1 > x2) || (y1 > y2)) return;

	color = rgba_to_pixel(rgba);
	alpha = gfx_alpha(rgba);

	dst_line = scr_adr + scr_width*y1;

	/* solid fill for 100% alpha */
	if (alpha == 0xff) {
		for (y = y1; y <= y2; y++, dst_line += scr_width)
			for (dst = dst_line + x1, x = x1; x <= x2; x++, dst++)
				*dst = color;

	/* mix colors for 50% alpha */
	} else if (alpha == 0x7f) {
		color = (color >> 1) & 0x7bef;    /* 50% alpha mode */
		for (y = y1; y <= y2; y++, dst_line += scr_width)
			for (dst = dst_line + x1, x = x1; x <= x2; x++, dst++)
				*dst = ((*dst >> 1) & 0x7bef) + color;

	/* mix colors for any other alpha values */
	} else {
		int max_minus_alpha = 255 - alpha;
		color = blend(color, alpha);
		for (y = y1; y <= y2; y++, dst_line += scr_width)
			for (dst = dst_line + x1, x = x1; x <= x2; x++, dst++)
				*dst = blend(*dst, max_minus_alpha) + color;
	}
}


static void scr_draw_slice(struct gfx_ds_data *s, int x, int y, int w, int h,
                           int sx, int sy, int sw, int sh,
                           struct gfx_ds *img, u8 alpha)
{
	enum img_type type  = img->handler->get_type(img->data);
	int           img_w = img->handler->get_width(img->data);

	switch (type) {
	case GFX_IMG_TYPE_RGB16:
		{
			pixel_t *src = (pixel_t *)img->handler->map(img->data);
			paint_scaled_img(x, y, w, h, img_w, sw, sh, src + img_w*sy + sx);
			break;
		}

	case GFX_IMG_TYPE_RGBA32:
		{
			u32 *src = (u32 *)img->handler->map(img->data);
			paint_scaled_img_rgba32(x, y, w, h, img_w, sw, sh, src + img_w*sy + sx);
			break;
		}

	default: break;
	}
}


static void scr_draw_img(struct gfx_ds_data *s, int x, int y, int w, int h,
                         struct gfx_ds *img, u8 alpha)
{
	scr_draw_slice(s, x, y, w, h, 0, 0,
	               img->handler->get_width(img->data),
	               img->handler->get_height(img->data),
	               img, alpha);
}


/**
 * Draw line of glyph using anti-aliasing values from the font image
 */
static inline void draw_glyph_line(u8 *src_alpha, u16 color, u16 *dst, int len)
{
	int i;
	for (i = 0; i < len; i++) {
		u8 alpha = src_alpha[i];
		if (alpha) {
			if (alpha == 255)
				dst[i] = color;
			else
				dst[i] = blend(dst[i], 255 - alpha)
				+ blend(rgba_to_pixel(color), alpha);
		}
	}
}


static void scr_draw_string(struct gfx_ds_data *ds, int x, int y,
                            color_t fg_rgba, color_t bg_rgba, int fnt_id,
                            char *str_signed)
{
	struct font *font = fontman->get_by_id(fnt_id);
	s32         *wtab = font->width_table;
	s32         *otab = font->offset_table;
	s32         img_w = font->img_w;
	s32         img_h = font->img_h;
	pixel_t     *dst = scr_adr + y*scr_width + x;
	u8          *str = (u8 *)str_signed;
	u8          *src = font->image;
	u8          *s;
	pixel_t     *d;
	int          j;
	int          w;
	int          h = font->img_h;
	pixel_t      color = rgba_to_pixel(fg_rgba);

	if (!str) return;

	/* check top clipping */
	if (y < clip_y1) {
		src += (clip_y1 - y)*img_w;   /* skip upper lines in font image */
		h   -= (clip_y1 - y);         /* decrement number of lines to draw */
		dst += (clip_y1 - y)*scr_width;
	}

	/* check bottom clipping */
	if (y+img_h - 1 > clip_y2)
		h -= (y + img_h - 1 - clip_y2);  /* decr. number of lines to draw */

	if (h < 1) return;

	/* skip characters that are completely hidden by the left clipping border */
	while (*str && (x+wtab[(int)(*str)] < clip_x1)) {
		x+=wtab[(int)(*str)];
		dst+=wtab[(int)(*str)];
		str++;
	}

	/* draw left cutted character */
	if (*str && (x+wtab[(int)(*str)] - 1 <= clip_x2) && (x < clip_x1)) {
		w = wtab[(int)(*str)] - (clip_x1 - x);
		s = src + otab[(int)(*str)] + (clip_x1 - x);
		d = dst + (clip_x1 - x);
		for (j = 0; j < h; j++) {
			draw_glyph_line(s, color, d, w);
			s = s + img_w;
			d = d + scr_width;
		}
		dst += wtab[(int)(*str)];
		x   += wtab[(int)(*str)];
		str++;
	}

	/* draw horizontally full visible characters */
	while (*str && (x + wtab[(int)(*str)] - 1 < clip_x2)) {
		w = wtab[(int)(*str)];
		s = src + otab[(int)(*str)];
		d = dst;
		for (j = 0; j < h; j++) {
			draw_glyph_line(s, color, d, w);
			s = s + img_w;
			d = d + scr_width;
		}
		dst += wtab[(int)(*str)];
		x   += wtab[(int)(*str)];
		str++;
	}

	/* draw right cutted character */
	if (*str) {
		w = wtab[(int)(*str)];
		s = src + otab[(int)(*str)];
		d = dst;
		if (x + w - 1 > clip_x2) {
			w -= x + w - 1 - clip_x2;
		}
		if (x < clip_x1) {    /* check if character is also left-cutted */
			w -= clip_x1 - x;
			s += clip_x1 - x;
			d += clip_x1 - x;
		}
		for (j = 0; j < h; j++) {
			draw_glyph_line(s, color, d, w);
			s += img_w;
			d += scr_width;
		}
	}
}


static void scr_push_clipping(struct gfx_ds_data *s, int x, int y, int w, int h)
{
	clip->push(x, y, x + w - 1, y + h - 1);
	clip_x1 = clip->get_x1();
	clip_y1 = clip->get_y1();
	clip_x2 = clip->get_x2();
	clip_y2 = clip->get_y2();
}


static void scr_pop_clipping(struct gfx_ds_data *s)
{
	clip->pop();
	clip_x1 = clip->get_x1();
	clip_y1 = clip->get_y1();
	clip_x2 = clip->get_x2();
	clip_y2 = clip->get_y2();
}


static void scr_reset_clipping(struct gfx_ds_data *s)
{
	clip->reset();
	clip_x1 = clip->get_x1();
	clip_y1 = clip->get_y1();
	clip_x2 = clip->get_x2();
	clip_y2 = clip->get_y2();
}


static int scr_get_clip_x(struct gfx_ds_data *s)
{
	return clip_x1;
}


static int scr_get_clip_y(struct gfx_ds_data *s)
{
	return clip_y1;
}


static int scr_get_clip_w(struct gfx_ds_data *s)
{
	return clip_x2 - clip_x1 + 1;
}


static int scr_get_clip_h(struct gfx_ds_data *s)
{
	return clip_y2 - clip_y1 + 1;
}


static void scr_set_mouse_pos(struct gfx_ds_data *s, int x, int y)
{
	scrdrv->set_mouse_pos(x, y);
}


static int register_gfx_handler(struct gfx_ds_handler *handler)
{
	handler->get_width      = scr_get_width;
	handler->get_height     = scr_get_height;
	handler->get_type       = scr_get_type;
	handler->destroy        = scr_destroy;
	handler->map            = scr_map;
	handler->update         = scr_update;
	handler->draw_hline     = scr_draw_hline;
	handler->draw_vline     = scr_draw_vline;
	handler->draw_fill      = scr_draw_fill;
	handler->draw_slice     = scr_draw_slice;
	handler->draw_img       = scr_draw_img;
	handler->draw_string    = scr_draw_string;
	handler->push_clipping  = scr_push_clipping;
	handler->pop_clipping   = scr_pop_clipping;
	handler->reset_clipping = scr_reset_clipping;
	handler->get_clip_x     = scr_get_clip_x;
	handler->get_clip_y     = scr_get_clip_y;
	handler->get_clip_w     = scr_get_clip_w;
	handler->get_clip_h     = scr_get_clip_h;
	handler->set_mouse_pos  = scr_set_mouse_pos;
	return 0;
}

#endif /* _DOPE_GFX_FUNCTIONS_H_ */
