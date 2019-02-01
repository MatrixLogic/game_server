trunk_path = $(shell pwd | sed "s/\/app.*//")
include $(trunk_path)/make.rules

INC =   -I. $(BASE_INC) $(FCGI_INC) $(DBC_INC) $(PENGYOU_INC) $(SAVE_INC) $(PROTO_INC) $(COMMON_INC) $(APP_PROTO_INC) $(CONFIG_INC)  $(KERNEL_INC) $(DATA_INC) $(BATTLE_INC) 
LIB	=   $(BASE_LIB) $(FCGI_LIB) $(DBC_LIB) $(PENGYOU_LIB) $(SAVE_LIB) $(PROTO_LIB) $(COMMON_LIB) $(APP_PROTO_LIB) $(CONFIG_LIB) $(KERNEL_LIB) $(DATA_LIB)  $(BATTLE_LIB)  

TARGETS = battled

all: $(TARGETS)
%:%.cpp
	$(CXX) $< $(CFLAGS) $(INC) -Xlinker "-(" $(LIB) -Xlinker  "-)" -o $@ -pthread -I/usr/local/include  -pthread -lprotobuf -lpthread  
#	cp $@ $(PATH_CGI) -f
clean:
	@rm -f *.d $(TARGETS) 
