#
# Build configuration for supporting the LatticeMico32 soft core
#

CROSS_PREFIX = lm32-elf-
CFLAGS      += -fomit-frame-pointer -mbarrel-shift-enabled -mmultiply-enabled -mdivide-enabled -msign-extend-enabled -fno-builtin -fsigned-char -fsingle-precision-constant
