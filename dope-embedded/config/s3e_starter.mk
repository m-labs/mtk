PLATFORM    = s3e_starter
EDK_PRJ_DIR = $(BASE_DIR)/../EDK_projects/$(PLATFORM)
CPU_FLAG    = -mcpu=v7.10.d

include $(BASE_DIR)/config/spec-microblaze.mk
include $(BASE_DIR)/config/spec-common.mk
