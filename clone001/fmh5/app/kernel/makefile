trunk_path = $(shell pwd | sed "s/\/app.*//")
include $(trunk_path)/app/applib.rules

INC := $(INC) $(COMMON_INC) $(APP_PROTO_INC) $(CONFIG_INC) 
LINK := $(LINK) $(COMMON_LIB) $(APP_PROTO_LIB) $(CONFIG_LIB) 

TARGET_DIR_NAME=kernel

include $(trunk_path)/app/applib.mk
