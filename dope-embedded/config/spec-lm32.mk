#
# Build configuration for supporting the LatticeMico32 soft core
#

CROSS_PREFIX = lm32-rtems4.11-
CFLAGS      += -mbarrel-shift-enabled -mmultiply-enabled -mdivide-enabled -msign-extend-enabled -fsigned-char
