trunk_path = $(shell pwd | sed "s/\/cgiapp.*//")
include $(trunk_path)/cgiapp/applib.rules

INC := $(INC) $(CGI_COMMON_INC) $(CGI_PLATFORM_INC)
LINK := $(LINK) $(CGI_COMMON_LIB) 

TARGET_DIR_NAME=kernel

include $(trunk_path)/cgiapp/applib.mk
