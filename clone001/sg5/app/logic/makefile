trunk_path = $(shell pwd | sed "s/\/app.*//")
include $(trunk_path)/app/applib.rules

INC := $(INC) $(COMMON_INC) $(PLATFORM_INC) $(KERNEL_INC) $(DATA_INC) 
LINK := $(LINK) $(COMMON_LIB) $(PLATFORM_LIB) $(KERNEL_LIB) $(DATA_LIB) 

TARGET_DIR_NAME=logic

include $(trunk_path)/app/applib.mk
