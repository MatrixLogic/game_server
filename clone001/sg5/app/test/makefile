trunk_path = $(shell pwd | sed "s/\/app.*//")
include $(trunk_path)/make.rules

INC =   -I. $(BASE_INC) $(FCGI_INC) $(DBC_INC) $(PENGYOU_INC) $(COMMON_INC) $(KERNEL_INC) $(PLATFORM_INC) $(DATA_INC) $(LOGIC_INC) 
LIB =   $(BASE_LIB) $(FCGI_LIB) $(DBC_LIB) $(PENGYOU_LIB) $(COMMON_LIB) $(KERNEL_LIB) $(PLATFORM_LIB) $(DATA_LIB)  $(LOGIC_LIB) 

#-D_NEED_CHECK_OPEN

TARGETS =$(patsubst %.cpp,%,$(wildcard *.cpp))

all: $(TARGETS)

%:%.cpp
	$(CXX) $< $(CFLAGS) $(INC) -Xlinker "-(" $(LIB) -Xlinker "-)" -o $@ 
#	 cp $* $(PATH_CGI)/$(shell echo $*.fcgi | sed "s/cgi_//g" )
	cp $@ $(PATH_TOOL) -f

%:%.c
	$(CXX) $(CFLAGS) -o $@ $< $(INC) $(LIB)
#	 cp $* $(PATH_CGI)/$*

clean:
	@rm -f $(TARGETS)

