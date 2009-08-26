#
# \brief  Generic part of the test application's makefile
# \author Norman Feske
# \date   2007-12-03
#
# The following variables must be defined before
# including this file:
#
# BASE_DIR     - base directory of DOpE-embedded source tree
# CROSS_PREFIX - cross-compiler prefix
# PLATFORM     - name of target platform
#

TARGET       = dopetest
TEST_DIR     = $(BASE_DIR)/test
DOPE_LIB_DIR = $(BASE_DIR)/lib/$(PLATFORM)
SRC_C       += test.c disp_img.c settings.c grid.c dopecmd.c colors.c \
               slides.c slideshow.c led.c
LIBS        += -ldope
CFLAGS      += -I. -I$(TEST_DIR) -I$(BASE_DIR)/include
CFLAGS      += -Wall -O2 -MMD -g -Wtrigraphs
LDFLAGS     += -L$(DOPE_LIB_DIR)
OBJECTS      = $(SRC_S:.S=.o) $(SRC_C:.c=.o)

vpath test.c      $(TEST_DIR)
vpath disp_img.c  $(TEST_DIR)
vpath settings.c  $(TEST_DIR)
vpath grid.c      $(TEST_DIR)
vpath dopecmd.c   $(TEST_DIR)
vpath colors.c    $(TEST_DIR)
vpath slides.c    $(TEST_DIR)
vpath slideshow.c $(TEST_DIR)
vpath led.c       $(TEST_DIR)

#
# Default rule
#
$(TARGET):

#
# Revisit the DOpE libary on each build
#
.PHONY: $(DOPE_LIB_DIR)/libdope.a

$(DOPE_LIB_DIR)/libdope.a:
	make -C $(DOPE_LIB_DIR)

#
# Build target
#
$(TARGET): $(OBJECTS) $(DOPE_LIB_DIR)/libdope.a
	$(LD) $(LDFLAGS) -Wl,--start-group $(OBJECTS) $(LIBS) -Wl,--end-group -o $@

$(TARGET)-stripped: $(TARGET)
	cp $< $@
	$(STRIP) $@

$(TARGET)-raw: $(TARGET)
	$(OBJCOPY) -O binary $< $@

#
# Cleanup
#
clean::
	rm -f $(TARGET) $(TARGET)-stripped $(TARGET)-raw $(OBJECTS) *.d

-include *.d
