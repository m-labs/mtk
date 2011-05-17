#
# \brief  Generic part of the libmtk makefile.
#
# The following variables must be defined before
# including this file:
#
# BASE_DIR     - Base directory of MTK source tree
# CROSS_PREFIX - Cross-compiler prefix
#

TARGET      = libmtk.a
LIBMTK_DIR = $(BASE_DIR)/lib
CFLAGS     += -I$(LIBMTK_DIR) -I$(BASE_DIR)/include
CFLAGS     += -Wall -O9 -MMD -g

SRC_C  += \
	appman.c      background.c  button.c    \
	cache.c       clipping.c    container.c \
	conv_fnt.c    mtkstd.c     entry.c     \
	fontman.c     frame.c       gfx.c       \
	gfx_img16.c   gfx_img32.c   \
	widman.c      conv_tff.c    grid.c      \
	hashtab.c     label.c     \
	loaddisplay.c pool.c      \
	winlayout.c   redraw.c      scale.c     \
	window.c      screen.c      script.c    \
	scrollbar.c   tick.c      \
	tokenizer.c   userstate.c   variable.c  \
	scope.c   \
	bigmouse.c    default_fnt.c main.c      \
	smallmouse.c  mono_fnt.c    title_fnt.c \
	scrdrv.c      eventmsg.c  \
	sharedmem.c   gfx_scr16.c   scheduler.c \
	vera16_tff.c  vera20_tff.c  edit.c \
	separator.c   pixmap.c      list.c \
	clipboard.c

vpath % $(LIBMTK_DIR)

$(TARGET): $(SRC_C:.c=.o)
	$(AR) -r $@ $^

clean:
	rm -f $(TARGET) $(SRC_C:.c=.o) *.d

-include *.d
