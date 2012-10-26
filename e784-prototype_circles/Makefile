CPP_SOURCES = main.cpp
C_SOURCES   = agent_functions.c
HEADERS     = agent_functions.h message_datatypes.h

FLAME2_DIR=/home/lsc/work/build/flame2
# Based on $(includedir) and $(pkgincludedir) and @COMMON_CPPFLAGS@
FLAME2_CPPFLAGS = -I$(FLAME2_DIR)/include -I$(FLAME2_DIR)/include/flame2 -I/usr/include/libxml2
FLAME2_LDFLAGS = -L$(FLAME2_DIR)/lib -lflame2 -lboost_filesystem -lboost_system -lxml2 -lboost_thread

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

