trunk_path = $(shell pwd | sed "s/\/save.*//")
include $(trunk_path)/make.rules

TARGET_DIR_NAME=save

CUR_PATH = $(trunk_path)/$(TARGET_DIR_NAME)
CPPS = $(wildcard $(CUR_PATH)/*.cc )
OBJS = $(patsubst $(CUR_PATH)/%.cc, ./%.o, $(CPPS))
TARGET = libsg17$(TARGET_DIR_NAME).so

all	: $(TARGET) 

clean : 
	rm -f $(TARGET) *.o *.d

$(TARGET) : $(OBJS)
	$(CXX) $(OBJS) $(CXXFLAGS) $(INC) -Xlinker "-(" $(LINK) -Xlinker "-)" -shared -o $(TARGET)
#	cp $@ $(PATH_CGI) -f

%.o : $(CUR_PATH)/%.cc
	$(CXX) $(INC) -c $(CXXFLAGS) $< -o  $@

