CC         = g++
CFLAGS     = -g -Wall# -pedantic
BOOST_DIR  = /Users/stc/workspace/boost_1_49_0
LDFLAGS    = -L$(BOOST_DIR)/stage/lib
LIBS       = -lboost_unit_test_framework
INCLUDE    = -I$(BOOST_DIR)
SOURCES    = xvariable.cpp xadt.cpp xtimeunit.cpp xfunction.cpp xmachine.cpp \
              xmessage.cpp xmodel.cpp io_xml_model.cpp xioput.cpp \
              tests/test_io_xml_model.cpp
OBJECTS    = $(SOURCES:.cpp=.o)
HEADERS    = io_xml_model.hpp xmachine.hpp xmodel.hpp xvariable.hpp xadt.hpp \
              xtimeunit.hpp xfunction.hpp xmessage.hpp xioput.hpp
DEPS       = Makefile $(HEADERS)
EXECUTABLE = tests/test_io_xml_model
RM         = rm -f

all: $(SOURCES) $(EXECUTABLE)
	
$(EXECUTABLE): $(OBJECTS) 
	$(CC) $(LDFLAGS) $(OBJECTS) -o $@ $(LIBS)

$(OBJECTS): $(DEPS)

.cpp.o:
	$(CC) -c $(CFLAGS) $(INCLUDE) $< -o $@

clean:
	$(RM) *.o tests/*.o $(EXECUTABLE)

CPPLINT   = ../tests/cpplint.py
lint:
	$(CPPLINT) $(SOURCES) $(HEADERS)

# dsymutil - only relevent on Mac OS X
memtest: all
	valgrind --dsymutil=yes --leak-check=full $(EXECUTABLE)
	rm -fr $(EXECUTABLE).dSYM

# To run executable:
# export DYLD_LIBRARY_PATH=/Users/stc/workspace/boost_1_49_0/stage/lib