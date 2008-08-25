#
# Build configuration for supporting the Xilinx Microblaze soft core
#

EDK_CPU_DIR  = $(EDK_PRJ_DIR)/microblaze_0
CPU_FLAG     = -mcpu=v7.00.a
CROSS_PREFIX = mb-
CFLAGS      += -mno-xl-soft-mul -mxl-barrel-shift \
               -mxl-pattern-compare -mno-xl-soft-div \
               $(CPU_FLAG) \
               -I$(EDK_CPU_DIR)/include
LDFLAGS     += -L$(EDK_CPU_DIR)/lib
