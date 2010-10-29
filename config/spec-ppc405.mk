#
# Build configuration for supporting the IBM PowerPC 405
# (as found in Xilinx Virtex4 devices)
#

EDK_CPU_DIR  = $(EDK_PRJ_DIR)/ppc405_0
CPU_FLAG     = -mcpu=405
CROSS_PREFIX = powerpc-eabi-
CFLAGS      += $(CPU_FLAG) \
               -I$(EDK_CPU_DIR)/include
LDFLAGS     += -L$(EDK_CPU_DIR)/lib
