CC         = g++
CFLAGS     = -g -Wall# -pedantic
BOOST_DIR  = /Users/stc/workspace/boost_1_49_0
LIBXML_DIR = /usr/include/libxml2
LDFLAGS    = -L$(BOOST_DIR)/stage/lib
LIBS       = -lboost_unit_test_framework -lboost_system -lboost_filesystem \
             -lxml2
INCLUDE    = -I$(BOOST_DIR) -I$(LIBXML_DIR)
SOURCES    = xvariable.cpp xadt.cpp xtimeunit.cpp xfunction.cpp xmachine.cpp \
              xmessage.cpp xmodel.cpp io_xml_model.cpp xioput.cpp \
              xcondition.cpp xmodel_validate.cpp tests/test_io_xml_model.cpp \
              io_xml_pop.cpp
OBJECTS    = $(SOURCES:.cpp=.o)
HEADERS    = io_xml_model.hpp xmachine.hpp xmodel.hpp xvariable.hpp xadt.hpp \
              xtimeunit.hpp xfunction.hpp xmessage.hpp xioput.hpp \
              xcondition.hpp io_xml_pop.hpp
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