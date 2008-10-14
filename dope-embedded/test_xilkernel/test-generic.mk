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
TEST_DIR     = $(BASE_DIR)/test_xilkernel
DOPE_LIB_DIR = $(BASE_DIR)/lib/$(PLATFORM)_xil
SRC_C       += test.c disp_img.c
LIBS        += -ldope -lxilkernel
CFLAGS      += -I. -I$(TEST_DIR) -I$(BASE_DIR)/include
CFLAGS      += -Wall -O2 -MMD -g -Wtrigraphs
CFLAGS      += -D__XMK__
LDFLAGS     += -L$(DOPE_LIB_DIR)
OBJECTS      = $(SRC_C:.c=.o)

vpath test.c $(TEST_DIR)
vpath disp_img.c $(TEST_DIR)

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
	$(LD) $(LDFLAGS) $(OBJECTS) $(LIBS) -o $@

$(TARGET)-stripped: $(TARGET)
	cp $< $@
	$(STRIP) $@

#
# Cleanup
#
clean::
	rm -f $(TARGET) $(TARGET)-stripped $(OBJECTS) *.d

-include *.d
