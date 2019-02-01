trunk_path = $(shell pwd | sed "s/\/cgiapp.*//")
include $(trunk_path)/cgiapp/applib.rules

INC := $(INC) $(CGI_COMMON_INC) $(CGI_KERNEL_INC)
LINK := $(LINK) $(CGI_COMMON_LIB) $(CGI_KERNEL_LIB)

TARGET_DIR_NAME=data

include $(trunk_path)/cgiapp/applib.mk
