/*
 * \brief   Viewer for tff (trivial font format) font files
 * \date    2005-11-07
 * \author  Norman Feske
 */

#include <SDL/SDL.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>

typedef unsigned char  u8;
typedef unsigned short u16;
typedef unsigned long  u32;


/**
 * Multiply 16bit color with alpha value
 */
static inline u16 blend(u16 color, int alpha)
{
	return ((((alpha >> 3) * (color & 0xf81f)) >> 5) & 0xf81f)
	      | (((alpha * (color & 0x07e0)) >> 8) & 0x7e0);
}


/**
 * Find highest value in array
 */
static int highest(u32 *buf, int num)
{
	int i, max = 0;
	for (i = 0; i < num; i++)
		if (buf[i] > max) max = buf[i];
	return max;
}


/**
 * Draw single character
 */
static void draw_char(u8 *src, int src_w, int w, int h, u16 *dst, int dst_w)
{
	int x, y;
	for (y = 0; y < h; y++)
		for (x = 0; x < w; x++)
			dst[y*dst_w + x] = blend(0xffff, src[y*src_w + x]);
}


/**
 * Draw a tinted box
 */
static void tint(u16 *dst, int dst_w, int w, int h, u16 color)
{
	int x, y;
	for (y = 0; y < h; y++)
		for (x = 0; x < w; x++)
			dst[y*dst_w + x] |= color;
}


/**
 * Main program
 */
int main(int argc, char **argv)
{
	SDL_Surface *screen;
	SDL_Event    event;
	u8  *tff;
	u16 *scr_adr;
	int  x, y;
	u32  otab[256], wtab[256];
	u8  *img;
	u32  cw, w, h;

	if (argc < 2) {
		printf("\nusage:\n\n  tffview <fontfile>\n\n");
		return 0;
	}

	int fh = open(argv[1], O_RDONLY);
	if (fh < 0) {
		printf("Error: Could not open file %d for reading.\n", (int)argv[1]);
		return -1;
	}

	int cnt = 0;
	cnt += read(fh, otab, sizeof(otab));
	cnt += read(fh, wtab, sizeof(wtab));
	cnt += read(fh, &w,   sizeof(w));
	cnt += read(fh, &h,   sizeof(h));

	if (cnt != 1024*2 + 8) {
		printf("Error: Could not read %d bytes from font file\n", 1024*2 + 8);
		return -1;
	}

	cw = highest(wtab, 256);

	printf("Size of font image is %dx%d, character cell is %dx%d.\n",
	       (int)w, (int)h, (int)cw, (int)h);

	if (!(img = malloc(w*h))) {
		printf("Error: Could not allocate %d bytes for font image\n", (int)(w*h));
		return -1;
	}

	if (read(fh, img, w*h) != w*h) {
		printf("Error: Could not read font image from font file\n");
		return -1;
	}

	if (SDL_Init(SDL_INIT_VIDEO) < 0) return 0;

	screen = SDL_SetVideoMode(cw*16, h*16, 16, SDL_SWSURFACE);
	scr_adr = (u16 *)(screen->pixels);

	for (y = 0; y < 16; y++)
		for (x = 0; x < 16; x++) {
			int n = y*16 + x;
			u16 *d = scr_adr + x*cw + y*16*cw*h;
			draw_char(img + otab[n], w, wtab[n], h, d, cw*16);
			tint(d, cw*16, cw, h, (x + y) & 1 ? 0 : 0x3800);
		}

	SDL_UpdateRect(screen, 0, 0, 16*cw, 16*h);

	while (1) {
		SDL_WaitEvent(&event);
		switch (event.type) {
			case SDL_KEYDOWN:
			case SDL_QUIT:
				return 0;
		}
	}

	return 0;
}
