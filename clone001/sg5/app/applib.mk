CUR_PATH = $(trunk_path)/app/$(TARGET_DIR_NAME)
CPPS = $(wildcard $(CUR_PATH)/*.cpp )
OBJS = $(patsubst $(CUR_PATH)/%.cpp, ./%.o, $(CPPS))
TARGET = libsgonline$(TARGET_DIR_NAME).so

all	: $(TARGET) 

clean : 
	rm -f $(TARGET) *.o

$(TARGET) : $(OBJS)
	$(CXX) $(OBJS) $(CXXFLAGS) $(INC) -Xlinker "-(" $(LINK) -Xlinker "-)" -shared -o $(TARGET)
	cp $@ $(PATH_CGI) -f

%.o : $(CUR_PATH)/%.cpp
	$(CXX) $(INC) -c $(CXXFLAGS) $< -o  $@
