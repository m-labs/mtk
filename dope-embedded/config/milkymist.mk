PLATFORM  = milkymist
CFLAGS   += -nostdinc -I$(MMDIR)/software/include -DNO_DOUBLE
LIBS     += $(MMDIR)/software/baselib/baselib.a


include $(BASE_DIR)/config/spec-lm32.mk
include $(BASE_DIR)/config/spec-common.mk
