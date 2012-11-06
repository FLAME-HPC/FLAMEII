CPP_SOURCES = main.cpp
C_SOURCES   = agent_functions.c
HEADERS     = agent_functions.h message_datatypes.h

FLAME2_CONFIG = flame2-config

COMPILE_FLAGS = $(shell $(FLAME2_CONFIG) --cflags)
LINK_COMMAND = libtool --tag=CXX --mode=link --silent $(CXX)
LINK_FLAGS = $(shell $(FLAME2_CONFIG) --libs --rpath)
#LINK_FLAGS = $(shell $(FLAME2_CONFIG) --libs --rpath --static)  # force static linking

EXECUTABLE = run

DEPS      = $(HEADERS) Makefile
OBJECTS   = $(C_SOURCES:.c=.o) $(CPP_SOURCES:.cpp=.o)

.PHONY: f2config

all: f2config $(EXECUTABLE)

# make sure flame2-config tool can be run
f2config:
	@$(FLAME2_CONFIG) --version > /dev/null

$(EXECUTABLE): $(OBJECTS)
	@echo "Linking executable '$@'"
	@$(LINK_COMMAND)  $(OBJECTS) -o $@ $(LINK_FLAGS)

$(OBJECTS): $(DEPS)

# Use C++ compiler, even for C code
.c.o:
	@echo "Compiling $<"
	@$(CXX) -c $(COMPILE_FLAGS) $< -o $@

.cpp.o:
	@echo "Compiling $<"
	@$(CXX) -c $(COMPILE_FLAGS) $< -o $@
	
clean:
	rm -f $(EXECUTABLE) $(OBJECTS)

