CC         = g++
CFLAGS     = -g -Wall -DTESTBUILD# -pedantic
BOOST_DIR  = /Users/stc/workspace/boost_1_49_0
LIBXML_DIR = /usr/include/libxml2
LDFLAGS    = -L$(BOOST_DIR)/stage/lib
LIBS       = -lboost_unit_test_framework -lboost_system -lboost_filesystem \
             -lxml2
INCLUDE    = -I$(BOOST_DIR) -I$(LIBXML_DIR)
TEST_IO_MODEL = src/io/tests/test_io_xml_model.cpp
TEST_IO_POP   = src/io/tests/test_io_xml_pop.cpp
SOURCES    = src/io/xvariable.cpp src/io/xadt.cpp src/io/xtimeunit.cpp src/io/xfunction.cpp src/io/xmachine.cpp \
              src/io/xmessage.cpp src/io/xmodel.cpp src/io/io_xml_model.cpp src/io/xioput.cpp \
              src/io/xcondition.cpp src/io/xmodel_validate.cpp src/io/io_xml_pop.cpp src/mem/memory_manager.cpp \
              run_tests.cpp $(TEST_IO_POP) $(TEST_IO_MODEL)
OBJECTS    = $(SOURCES:.cpp=.o)
HEADERS    = src/io/io_xml_model.hpp src/io/xmachine.hpp src/io/xmodel.hpp src/io/xvariable.hpp src/io/xadt.hpp \
              src/io/xtimeunit.hpp src/io/xfunction.hpp src/io/xmessage.hpp src/io/xioput.hpp \
              src/io/xcondition.hpp src/io/io_xml_pop.hpp src/mem/memory_manager.hpp
DEPS       = Makefile $(HEADERS)
EXECUTABLE = run_tests
RM         = rm -f

all: $(SOURCES) $(EXECUTABLE)
	
$(EXECUTABLE): $(OBJECTS) 
	$(CC) $(LDFLAGS) $(OBJECTS) -o $@ $(LIBS)

$(OBJECTS): $(DEPS)

.cpp.o:
	$(CC) -c $(CFLAGS) $(INCLUDE) $< -o $@

clean:
	$(RM) $(OBJECTS) $(EXECUTABLE)

CPPLINT   = src/tests/cpplint.py
lint:
	$(CPPLINT) $(SOURCES) $(HEADERS)

# dsymutil - only relevent on Mac OS X
memtest: all
	valgrind --dsymutil=yes --leak-check=full ./$(EXECUTABLE)
	rm -fr $(EXECUTABLE).dSYM

# To run executable:
# export DYLD_LIBRARY_PATH=/Users/stc/workspace/boost_1_49_0/stage/lib