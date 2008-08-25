#
# Build configuration for supporting libSDL on Linux
#

PLATFORM = sdl
LDFLAGS += -lSDL

include $(BASE_DIR)/config/spec-common.mk
