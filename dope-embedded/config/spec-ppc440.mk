#
# Build configuration for supporting the IBM PowerPC 440 
# (as found in Xilinx Virtex5 devices)
#

EDK_CPU_DIR  = $(EDK_PRJ_DIR)/ppc440_0
CPU_FLAG     = -mcpu=440
CROSS_PREFIX = powerpc-eabi-
CFLAGS      += $(CPU_FLAG) \
               -I$(EDK_CPU_DIR)/include
LDFLAGS     += -L$(EDK_CPU_DIR)/lib
