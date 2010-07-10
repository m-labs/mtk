PLATFORM  = milkymist
CFLAGS   += -I$(RTEMS_MAKEFILE_PATH)/lib/include 


include $(BASE_DIR)/config/spec-lm32.mk
include $(BASE_DIR)/config/spec-common.mk
