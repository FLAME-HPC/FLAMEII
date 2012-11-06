CPP_SOURCES = main.cpp
C_SOURCES   = agent_functions.c
HEADERS     = agent_functions.h message_datatypes.h

# Required scripts
FLAME2_CONFIG = flame2-config
LIBTOOL = flame2-libtool

# Compiler flags for different modes
COMPILE_FLAGS_COMMON = -Wall
COMPILE_FLAGS_DBG = $(shell $(FLAME2_CONFIG) --cflags)
COMPILE_FLAGS_PROD = $(shell $(FLAME2_CONFIG) --cflags --production)

# set default build type to devel
MODE = devel
COMPILE_FLAGS = $(COMPILE_FLAGS_COMMON) $(COMPILE_FLAGS_DBG)

# command and flags to link binary to libflame2
LINK_COMMAND = $(LIBTOOL) --tag=CXX --mode=link --silent $(CXX)
LINK_FLAGS = $(shell $(FLAME2_CONFIG) --libs --rpath)

# conditionally force libtool to statically link all depencies
ifeq ($(static),1)
	LINK_FLAGS = $(shell $(FLAME2_CONFIG) --libs --static) 
endif

EXECUTABLE = run
DEPS      = $(HEADERS) Makefile
OBJECTS   = $(C_SOURCES:.c=.o) $(CPP_SOURCES:.cpp=.o)

.PHONY: f2config show production

# default rule.
all: f2config $(EXECUTABLE)

# make sure flame2-config and libtool can be run
f2config:
	@$(FLAME2_CONFIG) --version > /dev/null
	@$(LIBTOOL) --version > /dev/null

# alternative build target
production: MODE = production
production: COMPILE_FLAGS = $(COMPILE_FLAGS_COMMON) $(COMPILE_FLAGS_PROD)
production: $(EXECUTABLE)
	
show:
	@echo "Compiler: $(CXX)"
	@echo "Link command: $(LINK_COMMAND)"
	@echo ""
	@echo "Devel compile flags: $(COMPILE_FLAGS_COMMON) $(COMPILE_FLAGS_DBG)"
	@echo ""
	@echo "Production compile flags: $(COMPILE_FLAGS_COMMON) $(COMPILE_FLAGS_PROD)"
	@echo ""
	@echo "Linker flags: $(LINK_FLAGS)"

$(EXECUTABLE): $(OBJECTS)
ifeq ($(static),1)
	@echo "Statically linking executable '$@'"
else
	@echo "Linking executable '$@'"
endif
	@$(LINK_COMMAND)  $(OBJECTS) -o $@ $(LINK_FLAGS)

$(OBJECTS): $(DEPS)

# Use C++ compiler, even for C code
.c.o:
	@echo "[$(MODE)] Compiling $<"
	@$(CXX) -c $(COMPILE_FLAGS) $< -o $@

.cpp.o:
	@echo "[$(MODE)] Compiling $<"
	@$(CXX) -c $(COMPILE_FLAGS) $< -o $@
	
clean:
	rm -f $(EXECUTABLE) $(OBJECTS)

