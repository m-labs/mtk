BASE_DIR = ../..
SRC_S    = crt0.S
SRC_C    = platform.c isr.c
LDFLAGS  = -nostdlib -nodefaultlibs -Wl,-T -Wl,dope.ld -Wl,-N

include $(BASE_DIR)/config/milkymist.mk
include ../test-generic.mk

#
# Convenience rules for uploading and testing
#
test: $(TARGET)-raw
	flterm --port /dev/ttyUSB0 --kernel $(TARGET)-raw --kernel-adr 0x40000000
