BASE_DIR = ../..
SRC_C    = platform.c
LDFLAGS  = -Wl,-T -Wl,dope.ld

include $(BASE_DIR)/config/ml405.mk
include ../test-generic.mk

#
# Convenience rules for uploading and testing
#
test: xmd.opt $(TARGET)-stripped
	xmd -opt xmd.opt

xmd.opt:
	echo "connect ppc hw"         >> $@
	echo "dow $(TARGET)-stripped" >> $@
	echo "run"                    >> $@

clean::
	rm -f xmd.opt
