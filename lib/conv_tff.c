/*
 * \brief   TFF (trivial font format) import module
 *
 * This module converts tff data to a generic font
 * structure that can be  used by other components
 * of MTK such as the font manager module.
 */

/*
 * Copyright (C) 2002-2009 Norman Feske <norman.feske@genode-labs.com>
 * Genode Labs, Feske & Helmuth Systementwicklung GbR
 *
 * This file is part of the MTK package, which is distributed
 * under the terms of the GNU General Public License version 2.
 */

#include "mtkstd.h"
#include "fontconv.h"

/**
 * File header structure of *.tff files
 */
struct tff_file_hdr {
	u32   otab[256];   /* glyph offset table   */
	u32   wtab[256];   /* glyph width table    */
	u32   img_w;       /* width of font image  */
	u32   img_h;       /* height of font image */
};

int init_conv_tff(struct mtk_services *d);


/********************************
 ** Functions for internal use **
 ********************************/

/**
 * Utility: convert intel 32bit value to host format
 */
static u32 i2u32(u32 *src)
{
	u8 *a = ((u8 *)src);
	u8 *b = ((u8 *)src) + 1;
	u8 *c = ((u8 *)src) + 2;
	u8 *d = ((u8 *)src) + 3;
	return ((u32)(*a))      + (((u32)(*b))<<8)
	    + (((u32)(*c))<<16) + (((u32)(*d))<<24);
}


/***********************
 ** Service functions **
 ***********************/

/**
 * Initialises and probes font
 */
static s16 font_probe(struct tff_file_hdr *tff)
{
	return 1;
}


/**
 * Generates width table for the specified font
 *
 * The width table will be generated at the given destination
 * address. It has one 32bit-entry per ASCII value so its size
 * is 256*4 bytes.
 */
static void font_gen_width_table(struct tff_file_hdr *tff, s32 *dst_wtab)
{
	u32 i;
	for (i = 0; i < 256; i++)
		dst_wtab[i] = i2u32(&tff->wtab[i]);
}


/**
 * Generates offset table for the specified font
 *
 * The offset table will be generated at the given destination
 * address. Its size is 256*4 bytes.
 */
static void font_gen_offset_table(struct tff_file_hdr *tff, s32 *dst_otab)
{
	u32 i;
	u32 offset;
	for (i = 0; i < 256; i++) {
		offset = i2u32(&tff->otab[i]);
		dst_otab[i] = offset;
	}
}


/**
 * Returns name of the font
 */
static char *font_get_name(struct tff_file_hdr *tff)
{
	return "nofontname";
}


/**
 * Get top line of font
 */
static u16 font_get_top(struct tff_file_hdr *tff)
{
	return 0;
}


/**
 * Get bottom line of font
 */
static u16 font_get_bottom(struct tff_file_hdr *tff)
{
	return 0;
}


/**
 * Get height of font image
 */
static u32 font_get_image_width(struct tff_file_hdr *tff)
{
	return i2u32(&tff->img_w);
}


/**
 * Get height of font image
 */
static u32 font_get_image_height(struct tff_file_hdr *tff)
{
	return i2u32(&tff->img_h);
}


/**
 * Generates chunky-organized font image
 *
 * The image will be generated at the specified
 * destination address such that the caller of
 * this routine has to take care about memory
 * allocation. The size of a font image is
 * image_width*image_height.
 */
static void font_gen_image(struct tff_file_hdr *tff, u8 *dst)
{
	u8 *src = (u8 *)tff;
	src += sizeof(*tff);
	int i, size = font_get_image_width(tff) * font_get_image_height(tff);

	for (i = 0; i < size; i++)
		*dst++ = *src++;
}


/**************************************
 ** Service structure of this module **
 **************************************/

static struct fontconv_services services = {
	(s16  (*) (void *))       font_probe,
	(void (*) (void *,s32 *)) font_gen_width_table,
	(void (*) (void *,s32 *)) font_gen_offset_table,
	(u8  *(*) (void *))       font_get_name,
	(u16  (*) (void *))       font_get_top,
	(u16  (*) (void *))       font_get_bottom,
	(u32  (*) (void *))       font_get_image_width,
	(u32  (*) (void *))       font_get_image_height,
	(void (*) (void *,u8 *))  font_gen_image,
};


/************************
 ** Module entry point **
 ************************/

int init_conv_tff(struct mtk_services *d)
{
	d->register_module("ConvertTFF 1.0", &services);
	return 1;
}
