trunk_path = $(shell pwd | sed "s/\/cgiapp.*//")
include $(trunk_path)/cgiapp/applib.rules

INC := $(INC) $(CGI_COMMON_INC) $(CGI_PLATFORM_INC) $(CGI_KERNEL_INC) $(CGI_DATA_INC) 
LINK := $(LINK) $(CGI_COMMON_LIB) $(CGI_PLATFORM_LIB) $(CGI_KERNEL_LIB) $(CGI_DATA_LIB) 

TARGET_DIR_NAME=logic

include $(trunk_path)/cgiapp/applib.mk
