CPP_SOURCES = main.cpp
C_SOURCES   = agent_functions.c
HEADERS     = agent_functions.h message_datatypes.h

FLAME2_DIR = /home/lsc/work/build/flame2
FLAME2_CONFIG = $(FLAME2_DIR)/bin/flame2-config
FLAME2_CPPFLAGS = `$(FLAME2_CONFIG) --cflags`
FLAME2_LDFLAGS = `$(FLAME2_CONFIG) --libs`

COV_FLAGS = -fprofile-arcs -ftest-coverage
CPPFLAGS  = -Wall $(FLAME2_CPPFLAGS)
LDFLAGS   = $(FLAME2_LDFLAGS) -lm

EXECUTABLE = run

DEPS      = $(HEADERS) Makefile
OBJECTS   = $(C_SOURCES:.c=.o) $(CPP_SOURCES:.cpp=.o)

all: $(EXECUTABLE)

$(EXECUTABLE): $(OBJECTS)
	$(CXX)  $(OBJECTS) $(LDFLAGS) -o $@

$(OBJECTS): $(DEPS)

# Use C++ compiler, even for C code
.c.o:
	$(CXX) -c $(CPPFLAGS) $< -o $@

.cpp.o:
	$(CXX) -c $(CPPFLAGS) $< -o $@
	
clean:
	rm -f $(EXECUTABLE) $(OBJECTS)

