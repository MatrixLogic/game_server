trunk_path = $(shell pwd | sed "s/\/app.*//")
include $(trunk_path)/app/applib.rules

INC := $(INC) $(APP_PROTO_INC) $(CONFIG_INC) 
LINK := $(LINK) $(APP_PROTO_LIB) $(CONFIG_LIB) 

TARGET_DIR_NAME=common

include $(trunk_path)/app/applib.mk
