
FT_CFLAGS = -I/usr/include/freetype2

all: tffview ttf2tff

tffview: tffview.c
	gcc -lSDL $^ -o $@

ttf2tff: ttf2tff.c
	gcc $(FT_CFLAGS) -lfreetype $^ -o $@
