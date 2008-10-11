PLATFORM    = s3a_starter
EDK_PRJ_DIR = $(BASE_DIR)/../EDK_projects/$(PLATFORM)
CPU_FLAG     = -mcpu=v7.00.a

include $(BASE_DIR)/config/spec-microblaze.mk
include $(BASE_DIR)/config/spec-common.mk
