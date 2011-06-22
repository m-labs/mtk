/*
 * \brief   MTK Button widget module
 *
 * This module converts fnt data to a generic font
 * structure that can be  used by other components
 * of MTK such as the font manager module.
 */

/*
 * Copyright (C) 2002-2008 Norman Feske <norman.feske@genode-labs.com>
 * Genode Labs, Feske & Helmuth Systementwicklung GbR
 *
 * This file is part of the MTK package, which is distributed
 * under the terms of the GNU General Public License version 2.
 */

#include "mtkstd.h"
#include "fontconv.h"

/**
 * File header structure of *.fnt files
 */
struct fntfile_hdr {
	u16   font_id;        /* font number                       */
	u16   point;          /* size                              */
	char  name[32];       /* font name                         */
	u16   first_ade;      /* first ascii char                  */
	u16   last_ade;       /* last ascii char                   */
	u16   top;            /* distance topline <-> baseline     */
	u16   ascent;         /* distance ascentline <-> baseline  */
	u16   half;           /* distance halfline <-> baseline    */
	u16   descent;        /* distance descentline <-> baseline */
	u16   bottom;         /* distance bottomline <-> baseline  */
	u16   max_char_width; /* biggest character width           */
	u16   max_cell_width; /* biggest character cell with       */
	u16   left_offset;    /* left offset for italic            */
	u16   right_offset;   /* right offset for italic           */
	u16   thicken;        /* scale value for bold              */
	u16   ul_size;        /* height of underscore              */
	u16   lighten;        /* mask for bright style             */
	u16   skew;           /* mask for italic                   */
	u16   flags;          /* fontflags                         */
	u32   hor_table;      /* pointer to horizontal table       */
	u32   off_table;      /* pointer to char offset table      */
	u32   dat_table;      /* pointer to font image             */
	u16   form_width;     /* width of font image               */
	u16   form_height;    /* height of font image              */
	u32   next_font;      /* unused                            */
};

int init_conv_fnt(struct mtk_services *d);


/********************************
 ** Functions for internal use **
 ********************************/

/**
 * Convert intel 16bit value to host format
 */
static u16 i2u16(u16 *src)
{
	u8 *low  = ((u8 *)src)+1;
	u8 *high = (u8 *)src;
	return (((u16)(*low))<<8) + (u16)(*high);
}


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


/**
 * Utility: convert motorola 16bit value to host format
 */
static u16 m2u16(u16 *src)
{
	u8 *low  = ((u8 *)src)+1;
	u8 *high =  (u8 *)src;
	return (((u16)(*high))<<8) + (u16)(*low);
}


/**
 * Utility: convert motorola 32bit value to host format
 */
static u32 m2u32(u32 *src)
{
	u8 *a = ((u8 *)src);
	u8 *b = ((u8 *)src) + 1;
	u8 *c = ((u8 *)src) + 2;
	u8 *d = ((u8 *)src) + 3;
	return ((u32)(*d))      + (((u32)(*c))<<8)
	    + (((u32)(*b))<<16) + (((u32)(*a))<<24);
}


/**
 * Utility: check if font is in intel format
 *
 * \return  1 if the font is in intel format
 *          0 if the font is in motorla format
 */
static inline int intel_format(struct fntfile_hdr *fnt)
{
	return !(fnt->flags & 4);
}


/***********************
 ** Service functions **
 ***********************/

/**
 * Initialises and probes font
 */
static s16 font_probe(struct fntfile_hdr *fnt)
{
	u32 max_char_w, img_w, img_h, point, first_ade, last_ade, top, bottom;
	u16 (*get_u16) (u16 *);
	INFO(char *dbg = "ConvertFNT(probe): ");

	if(intel_format(fnt)) {
		INFO(printf("%s font is in intel format\n", dbg));
		get_u16 = i2u16;
	} else  {
		INFO(printf("%s font is in motorola format\n", dbg));
		get_u16 = m2u16;
	}

	fnt->name[31] = 0;

	max_char_w = get_u16(&fnt->max_char_width);
	img_w      = get_u16(&fnt->form_width)*8;
	img_h      = get_u16(&fnt->form_height);
	point      = get_u16(&fnt->point);
	first_ade  = get_u16(&fnt->first_ade);
	last_ade   = get_u16(&fnt->last_ade);
	top        = get_u16(&fnt->top);
	bottom     = get_u16(&fnt->bottom);

	INFO(printf("%s image width:  %u\n",dbg,img_w));
	INFO(printf("%s image height: %u\n",dbg,img_h));
	INFO(printf("%s points:       %u\n",dbg,point));
	INFO(printf("%s first_ade:    %u\n",dbg,first_ade));
	INFO(printf("%s last_ade:     %u\n",dbg,last_ade));
	INFO(printf("%s top:          %u\n",dbg,top));
	INFO(printf("%s bottom:       %u\n",dbg,bottom));
	INFO(printf("%s name:         %s\n",dbg,fnt->name));

	/* strange values -> seems not to be a valid fnt */
	if(max_char_w > 100 || img_h < 1            || img_h > 100
	 || img_w < 1        || point > 50           || first_ade > 255
	 || last_ade > 255   || first_ade > last_ade || top > img_h
	 || bottom > img_h   || top < bottom) {

		INFO(printf("%sthis doesnt seem to be valid fnt data - sorry\n",dbg));
		return 0;
	}
	return 1;
}

/* http://www.kostis.net/charsets/atarist.htm */
static int iso8859_to_atari(int ch)
{
	switch(ch) {
		case 244: return 147;
		case 246: return 148;
		case 214: return 153;
		case 198: return 146;
		case 230: return 145;
		case 201: return 144;
		case 197: return 143;
		case 196: return 142;
		case 231: return 135;
		case 234: return 136;
		case 235: return 137;
		case 232: return 138;
		case 239: return 139;
		case 238: return 140;
		case 236: return 141;
		case 251: return 150;
		case 249: return 151;
		case 255: return 152;
		case 220: return 154;
		case 223: return 158;
		case 192: return 182;
		case 169: return 189;
		case 174: return 190;
		case 229: return 134;
		case 224: return 133;
		case 228: return 132;
		case 226: return 131;
		case 233: return 130;
		case 252: return 129;
		case 199: return 128;
		default: return ch;
	}
}

/**
 * Generates width table for the specified font
 *
 * The width table will be generated at the given destination
 * adress. It has one 32bit-entry per ASCII value so its size
 * is 256*4 bytes.
 */

static int get_width(struct fntfile_hdr *fnt, int ch)
{
	u16 first_ade, last_ade;
	u16 *offsets;
	u16 (*get_u16) (u16 *);
	u32 (*get_u32) (u32 *);

	if(intel_format(fnt)) {
		get_u16 = i2u16;
		get_u32 = i2u32;
	} else {
		get_u16 = m2u16;
		get_u32 = m2u32;
	}

	first_ade = get_u16(&fnt->first_ade);
	last_ade  = get_u16(&fnt->last_ade);
	offsets = (u16 *)((get_u32((u32 *)(&fnt->off_table))) + (int)fnt);
	
	if((ch >= first_ade) && (ch < last_ade))
		return (get_u16(offsets + ch + 1) - get_u16(offsets + ch));
	else
		return 0;
}

static void font_gen_width_table(struct fntfile_hdr *fnt, s32 *dst_wtab)
{
	int i;
	
	for(i=0;i<256;i++) 
		dst_wtab[i] = get_width(fnt, iso8859_to_atari(i));
}


/**
 * Generates offset table for the specified font
 *
 * The offset table will be generated at the given destination
 * adress. Its size is 256*4 bytes.
 */

static int get_offset(struct fntfile_hdr *fnt, int ch)
{
	u16   first_ade, last_ade;
	u16  *offsets;
	u16 (*get_u16) (u16 *);
	u32 (*get_u32) (u32 *);

	if(intel_format(fnt)) {
		get_u16 = i2u16;
		get_u32 = i2u32;
	} else {
		get_u16 = m2u16;
		get_u32 = m2u32;
	}

	first_ade = get_u16(&fnt->first_ade);
	last_ade  = get_u16(&fnt->last_ade);
	offsets   = (u16 *)((get_u32((u32 *)(&fnt->off_table))) + (int)fnt);
	
	if((ch >= first_ade) && (ch <= last_ade))
		return get_u16(offsets + ch);
	else
		return 0;
}

static void font_gen_offset_table(struct fntfile_hdr *fnt, s32 *dst_otab)
{
	int i;
	
	for(i=0;i<256;i++)
		dst_otab[i] = get_offset(fnt, iso8859_to_atari(i));
}


/**
 * Returns name of the font
 */
static char *font_get_name(struct fntfile_hdr *fnt)
{
	fnt->name[31] = 0;
	return (char *)&fnt->name;
}


/**
 * Get top line of font
 */
static u16 font_get_top(struct fntfile_hdr *fnt)
{
	if(intel_format(fnt))
		return i2u16(&fnt->top);
	else
		return m2u16(&fnt->top);
}


/**
 * Get bottom line of font
 */
static u16 font_get_bottom(struct fntfile_hdr *fnt)
{
	if(intel_format(fnt))
		return i2u16(&fnt->bottom);
	else
		return m2u16(&fnt->bottom);
}


/**
 * Get height of font image
 */
static u32 font_get_image_width(struct fntfile_hdr *fnt)
{
	if(intel_format(fnt))
		return 8*i2u16(&fnt->form_width);
	else
		return 8*m2u16(&fnt->form_width);
}


/**
 * Get height of font image
 */
static u32 font_get_image_height(struct fntfile_hdr *fnt)
{
	if(intel_format(fnt))
		return i2u16(&fnt->form_height);
	else
		return m2u16(&fnt->form_height);
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

static int getbit(u8 *array, int offset)
{
	return (array[offset / 8] & (0x80 >> (offset % 8))) ? 255 : 0;
}

static void font_gen_image(struct fntfile_hdr *fnt, u8 *dst)
{
	s32  linelength, height;
	u32  i, j, k;
	u8  *src;
	u16 (*get_u16) (u16 *);
	u32 (*get_u32) (u32 *);
	int ch;
	int offset;
	int width;

	if(intel_format(fnt)) {
		get_u16 = i2u16;
		get_u32 = i2u32;
	} else {
		get_u16 = m2u16;
		get_u32 = m2u32;
	}
	linelength = get_u16(&fnt->form_width);
	height     = get_u16(&fnt->form_height);
	src = (u8 *)((get_u32((u32 *)(&fnt->dat_table))) + (int)fnt);
	
	for(i=0;i<256;i++) {
		ch = iso8859_to_atari(i);
		width = get_width(fnt, ch);
		offset = get_offset(fnt, ch);
		for(j=0;j<height;j++)
			for(k=0;k<width;k++)
				dst[j*linelength*8+k] = getbit(src, offset+j*linelength*8+k);
		dst += width;
	}
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

int init_conv_fnt(struct mtk_services *d)
{
	d->register_module("ConvertFNT 1.0", &services);
	return 1;
}
