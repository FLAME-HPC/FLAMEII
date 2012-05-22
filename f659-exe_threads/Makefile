CC         = g++
CFLAGS     = -g -Wall -pedantic -DTESTBUILD
BOOST_DIR  = /Users/stc/workspace/boost_1_49_0
LDFLAGS    = -L$(BOOST_DIR)/stage/lib
LIBS       = -lboost_unit_test_framework -lboost_system -lboost_filesystem \
             $(shell xml2-config --libs)
INCLUDE    = -I. -I$(BOOST_DIR) $(shell xml2-config --cflags)
IO_SRC     = src/io/io_manager.cpp src/io/io_xml_model.cpp src/io/io_xml_pop.cpp
IO_TEST    = src/io/tests/test_io_xml_pop.cpp #src/io/tests/test_io_xml_model.cpp
MODEL_SRC  = src/model/xvariable.cpp src/model/xadt.cpp src/model/xtimeunit.cpp \
              src/model/xfunction.cpp src/model/xmachine.cpp \
              src/model/xmessage.cpp src/model/xmodel.cpp src/model/xioput.cpp \
              src/model/xcondition.cpp src/model/xmodel_validate.cpp \
              src/model/model_manager.cpp src/model/generate_task_list.cpp \
              src/model/task.cpp src/model/dependency.cpp
MODEL_TEST = src/model/tests/test_model_manager.cpp
#MEM_SRC    = src/mem/memory_manager.cpp src/mem/agent_memory.cpp
SOURCES    = $(IO_SRC) $(MODEL_SRC) $(MEM_SRC) run_tests.cpp $(IO_TEST) 
OBJECTS    = $(SOURCES:.cpp=.o)
HEADERS    = src/io/io_xml_model.hpp src/model/xmachine.hpp src/model/xmodel.hpp src/model/xvariable.hpp src/model/xadt.hpp \
              src/model/xtimeunit.hpp src/model/xfunction.hpp src/model/xmessage.hpp src/model/xioput.hpp \
              src/model/xcondition.hpp src/io/io_xml_pop.hpp src/mem/memory_manager.hpp \
              src/model/model_manager.hpp src/io/io_manager.hpp src/model/task.hpp src/model/dependency.hpp \
              src/exceptions/base.hpp src/exceptions/mem.hpp
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

CPPLINT   = /Users/stc/workspace/flame/scripts/cpplint.py
lint:
	$(CPPLINT) $(SOURCES) $(HEADERS)

# dsymutil - only relevent on Mac OS X
memtest: all
	valgrind --dsymutil=yes --leak-check=full ./$(EXECUTABLE)
	rm -fr $(EXECUTABLE).dSYM

# To run executable:
# export DYLD_LIBRARY_PATH=/Users/stc/workspace/boost_1_49_0/stage/lib