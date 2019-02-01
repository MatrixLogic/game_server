trunk_path = $(shell pwd | sed "s/\/cgiapp.*//")
include $(trunk_path)/make.rules

INC =   -I. $(BASE_INC) $(FCGI_INC) $(DBC_INC) $(PENGYOU_INC) $(SAVE_INC) $(PROTO_INC) $(CGI_COMMON_INC) $(CGI_KERNEL_INC) $(CGI_PLATFORM_INC) $(CGI_DATA_INC) $(CGI_LOGIC_INC) $(APP_PROTO_INC)
LIB =   $(BASE_LIB) $(FCGI_LIB) $(DBC_LIB) $(PENGYOU_LIB) $(SAVE_LIB) $(PROTO_LIB) $(CGI_COMMON_LIB) $(CGI_KERNEL_LIB) $(CGI_PLATFORM_LIB) $(CGI_DATA_LIB)  $(CGI_LOGIC_LIB) $(APP_PROTO_LIB)

TARGETS =$(patsubst %.cpp,%,$(wildcard *.cpp))

all: $(TARGETS)

%:%.cpp
	$(CXX) $< $(CFLAGS) $(INC) -Xlinker "-(" $(LIB) -Xlinker "-)" -o $@ -pthread -I/usr/local/include  -pthread -lprotobuf -lpthread  
#	cp $@ $(PATH_TOOL) -f

%:%.c
	$(CXX) $(CFLAGS) -o $@ $< $(INC) $(LIB)

clean:
	@rm -f $(TARGETS) *.d

