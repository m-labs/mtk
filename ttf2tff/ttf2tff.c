/*
 * \brief   Convert truetype fonts to trivial font format
 * \date    2005-11-07
 * \author  Norman Feske
 */

#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>
#include <ft2build.h>
#include FT_FREETYPE_H

typedef unsigned char  u8;
typedef unsigned short u16;
typedef unsigned long  u32;


void draw_bitmap(u8 *dst, int dst_w, int w, int h, u8 *src, int src_w) {
	int x, y;
	for (y = 0; y < h; y++) {
		for (x = 0; x < w; x++)
			dst[x] = src[x];
		src += src_w;
		dst += dst_w;
	}
}


static int max(int a, int b) { return a > b ? a : b; }


/**
 * Main program
 */
int main(int argc, char **argv)
{
	static FT_Library   library;
	static FT_Face      face;
	static FT_GlyphSlot slot;
	static u32 otab[256], wtab[256];
	u32    img_w, img_h;
	int    img_l;
	int    res, i;
	int    size = 16*64;
	u8    *img;

	if (argc != 4) {
		printf("\nConvert truetype fonts to trivial font format\n");
		printf("\nusage:\n\n  ttf2tff <source-ttf> <points> <destination-tff>\n\n");
		return 0;
	}

	if ((res = FT_Init_FreeType(&library))) {
		printf("Error: FT_Init_FreeType returned %d\n", res);
		return -1;
	}

	if ((res = FT_New_Face(library, argv[1], 0, &face))) {
		printf("Error: FT_New_Face returned %d - invalid font file?\n", res);
		return -1;
	}

	size = atof(argv[2])*64;

	printf("size       = %d\n",     size);
	printf("num_faces  = %d\n",     (int)face->num_faces);
	printf("num_glyphs = %d\n",     (int)face->num_glyphs);
	printf("face_flags = 0x08%x\n", (int)face->face_flags);

	slot = face->glyph;

	if ((res = FT_Set_Char_Size(face, 0, size, 0, 0))) {
		printf("Error: FT_Set_Char_Size returned %d\n", res);
		return;
	}

	/* calculate dimensions of font image */
	img_w = 0;
	img_h = 0;
	img_l = 0;
	for (i = 0; i < 256; i++) {
		int w, h, l;

		if (FT_Load_Char(face, i, FT_LOAD_RENDER | FT_LOAD_TARGET_LIGHT))
			continue;

		w = max(slot->bitmap.width, slot->advance.x >> 6);
		h = slot->bitmap.rows;
		l = slot->bitmap.rows - slot->bitmap_top;

		wtab[i] = w;
		otab[i] = img_w;

		img_w += w;
		img_h  = h > img_h ? h : img_h;
		img_l  = l > img_l ? l : img_l;
	}
	printf("font image is %dx%d (lowline at -%d) pixels\n",
	       (int)img_w, (int)img_h, (int)img_l);

	/* allocate font image */
	if (!(img = malloc(img_w*(img_h + 50)))) {
		printf("Error: Could not allocate %d bytes for font image\n", (int)(img_w*img_h));
		return -1;
	}
	bzero(img, img_w*img_h);

	/* draw glyphs into font image */
	int pen_x = 0;
	for (i = 0; i < 256; i++) {
		int x, y, w, h;
		u8 *s;

		if (FT_Load_Char(face, i, FT_LOAD_RENDER | FT_LOAD_TARGET_LIGHT))
			continue;

		s = slot->bitmap.buffer;
		x = otab[i];
		w = slot->bitmap.width;

		if (w < wtab[i]) {
			x += (wtab[i] - w) >> 1;
		}
		if (slot->bitmap_left < 0) {
			s -= slot->bitmap_left;
			w += slot->bitmap_left;
		}
		y = img_h - slot->bitmap_top;
		h = slot->bitmap.rows;

		draw_bitmap(img + x + y*img_w, img_w, w, h, s, slot->bitmap.pitch);

		pen_x += wtab[i];
	}

	img_h += img_l;

	/* write tff file */
	int fh = creat(argv[3], S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH);
	if (fh < 0) {
		printf("Error: Could not open file %s for writing\n", argv[3]);
		return -1;
	}

	int cnt = 0;
	cnt += write(fh,  otab,  sizeof(otab));
	cnt += write(fh,  wtab,  sizeof(wtab));
	cnt += write(fh, &img_w, sizeof(img_w));
	cnt += write(fh, &img_h, sizeof(img_h));
	cnt += write(fh,  img,   img_w*img_h);

	if (cnt != sizeof(otab) + sizeof(wtab) + sizeof(img_w) + sizeof(img_h) + img_w*img_h) {
		printf("Error: Write error\n");
		return -1;
	}
	close(fh);

	return 0;
}
