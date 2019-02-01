trunk_path = $(shell pwd | sed "s/\/app.*//")
include $(trunk_path)/app/applib.rules

INC := $(INC) $(COMMON_INC) $(KERNEL_INC) $(APP_PROTO_INC) $(CONFIG_INC) $(BATTLE_INC) 
LINK := $(LINK) $(COMMON_LIB) $(KERNEL_LIB) $(APP_PROTO_LIB) $(CONFIG_LIB) $(BATTLE_LIB)  

TARGET_DIR_NAME=data

include $(trunk_path)/app/applib.mk
