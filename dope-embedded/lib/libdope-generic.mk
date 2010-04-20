#
# \brief  Generic part of the libdope makefile.
# \author Norman Feske
# \date   2007-12-03
#
# The following variables must be defined before
# including this file:
#
# BASE_DIR     - Base directory of DOpE source tree
# CROSS_PREFIX - Cross-compiler prefix
#

TARGET      = libdope.a
LIBDOPE_DIR = $(BASE_DIR)/lib
CFLAGS     += -I$(LIBDOPE_DIR) -I$(BASE_DIR)/include
CFLAGS     += -Wall -O2 -MMD -g

SRC_C  += \
	appman.c      background.c  button.c    \
	cache.c       clipping.c    container.c \
	conv_fnt.c    dopestd.c     entry.c     \
	fontman.c     frame.c       gfx.c       \
	gfx_img16.c   gfx_img32.c   memory.c    \
	widman.c      conv_tff.c    grid.c      \
	hashtab.c     keymap.c      label.c     \
	loaddisplay.c relax.c       pool.c      \
	winlayout.c   redraw.c      scale.c     \
	window.c      screen.c      script.c    \
	scrollbar.c   vtextscreen.c tick.c      \
	tokenizer.c   userstate.c   variable.c  \
	vscreen.c     scope.c       viewman.c   \
	bigmouse.c    default_fnt.c main.c      \
	smallmouse.c  mono_fnt.c    title_fnt.c \
	dummies.c     thread.c      eventmsg.c  \
	sharedmem.c   gfx_scr16.c   sched.c     \
	vera16_tff.c  edit.c        separator.c

vpath % $(LIBDOPE_DIR)

$(TARGET): $(SRC_C:.c=.o)
	$(AR) -r $@ $^

clean:
	rm -f $(TARGET) $(SRC_C:.c=.o) *.d

-include *.d
