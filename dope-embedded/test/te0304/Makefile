BASE_DIR = ../..
SRC_C    = platform.c
LDFLAGS  = -Wl,-T -Wl,dope.ld

include $(BASE_DIR)/config/te0304.mk
include ../test-generic.mk

#
# Convenience rules for uploading and testing
#
test: xmd.opt $(TARGET)-stripped
	xmd -opt xmd.opt

xmd.opt:
	echo "connect mb mdm"         >> $@
	echo "dow $(TARGET)-stripped" >> $@
	echo "con"                    >> $@

clean::
	rm -f xmd.opt
