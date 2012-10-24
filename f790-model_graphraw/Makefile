CPPC       = g++
# -Wno-long-long is used to suppress long long integer warnings from Boost
CPPFLAGS   = -g -Wall -Wno-long-long -pedantic -DTESTBUILD -DDEBUG \
             -DOUTPUT_GRAPHS #-DUSE_VARIABLE_VERTICES
BOOSTDIR   = /Users/stc/workspace/boost/boost_1_48_0
LDFLAGS    = -L$(BOOSTDIR)/stage/lib
# Boost library naming support, -mt for multithreading, -d for debug
BOOSTLIB   = 
LIBS       = -lboost_unit_test_framework$(BOOSTLIB) \
             -lboost_system$(BOOSTLIB) \
             -lboost_filesystem$(BOOSTLIB) \
             -lboost_thread$(BOOSTLIB) \
             $(shell xml2-config --libs)
INCLUDE    = -I./headers -I./src $(shell xml2-config --cflags) -I$(BOOSTDIR)
IO_SRC     = src/io/io_manager.cpp \
             src/io/io_xml_model.cpp \
             src/io/io_xml_pop.cpp
IO_TEST_MAN   = src/io/tests/test_io_manager.cpp
IO_TEST_POP   = src/io/tests/test_io_xml_pop.cpp
IO_TEST_MODEL = src/io/tests/test_io_xml_model.cpp
IO_TEST    = $(IO_TEST_MAN) $(IO_TEST_POP) $(IO_TEST_MODEL)
COMPAT_SRC = src/compat/C/compatibility_manager.cpp \
             src/compat/C/flame_compat_c.cpp
MODEL_SRC  = src/model/xvariable.cpp \
             src/model/xadt.cpp \
             src/model/xtimeunit.cpp \
             src/model/xfunction.cpp \
             src/model/xmachine.cpp \
             src/model/xmessage.cpp \
             src/model/xmodel.cpp \
             src/model/xioput.cpp \
             src/model/xcondition.cpp \
             src/model/xmodel_validate.cpp \
             src/model/task.cpp \
             src/model/dependency.cpp \
             src/model/xgraph.cpp \
             src/model/model.cpp
MODEL_TEST = src/model/tests/test_xgraph.cpp \
             src/model/tests/test_model_validation.cpp
MEM_SRC    = src/mem/memory_manager.cpp \
             src/mem/agent_memory.cpp \
             src/mem/agent_shadow.cpp \
             src/mem/memory_iterator.cpp
MEM_TEST   = src/mem/tests/test_agent_memory.cpp \
             src/mem/tests/test_memory_iterator.cpp \
             src/mem/tests/test_memory_manager.cpp \
             src/mem/tests/test_vector_wrapper.cpp
EXE_SRC    = src/exe/agent_task.cpp \
             src/exe/fifo_task_queue.cpp \
             src/exe/io_task.cpp \
             src/exe/message_board_task.cpp \
             src/exe/scheduler.cpp \
             src/exe/splitting_fifo_task_queue.cpp \
             src/exe/task_manager.cpp \
             src/exe/task_splitter.cpp \
             src/exe/worker_thread.cpp
EXE_TEST   = src/exe/tests/test_execution_module.cpp \
             src/exe/tests/test_task_access_to_mb.cpp \
             src/exe/tests/test_task_manager.cpp \
             src/exe/tests/test_task_splitter.cpp
SIM_SRC    = src/sim/sim_manager.cpp \
             src/sim/simulation.cpp
SIM_TEST   = src/sim/tests/test_sim_manager.cpp \
             src/sim/tests/test_simulation.cpp
MB_SRC     = src/mb/board_writer.cpp \
             src/mb/client.cpp \
             src/mb/message_board_manager.cpp \
             src/mb/message_board.cpp \
             src/mb/message_iterator_backend_raw.cpp \
             src/mb/message_iterator_backend.cpp \
             src/mb/message_iterator.cpp \
             src/mb/message.cpp \
             src/mb/proxy.cpp
MB_TEST    = src/mb/tests/test_board_iteration.cpp \
             src/mb/tests/test_board_manager.cpp \
             src/mb/tests/test_board_proxy.cpp \
             src/mb/tests/test_board_writer.cpp
SOURCES    = $(COMPAT_SRC) $(IO_SRC) $(MODEL_SRC) $(MEM_SRC) \
             $(EXE_SRC) $(MB_SRC) $(SIM_SRC) run_tests.cpp \
             src/model/tests/test_xgraph.cpp
#             $(EXE_TEST) \
             $(MB_TEST) \
             $(MEM_TEST) \
             $(IO_TEST) \
             $(MODEL_TEST) \
             $(SIM_TEST)
OBJECTS    = $(SOURCES:.cpp=.o)
IO_MODEL_SIM_SRC_TEST = $(IO_SRC) $(IO_TEST) \
             $(MODEL_SRC) $(MODEL_TEST) $(SIM_SRC) $(SIM_TEST)
IO_MODEL_SIM_HEADERS = src/io/io_xml_model.hpp \
             src/io/io_xml_pop.hpp \
             src/io/io_manager.hpp \
             src/model/xmachine.hpp \
             src/model/xmodel.hpp \
             src/model/xmodel_validate.hpp \
             src/model/xvariable.hpp \
             src/model/xadt.hpp \
             src/model/xtimeunit.hpp \
             src/model/xfunction.hpp \
             src/model/xmessage.hpp \
             src/model/xioput.hpp \
             src/model/xcondition.hpp \
             src/model/task.hpp \
             src/model/dependency.hpp \
             src/model/xgraph.hpp \
             src/model/model.hpp \
             src/sim/sim_manager.hpp \
             src/sim/simulation.hpp
OTHER_HEADERS = src/compat/C/compatibility_manager.hpp \
             src/compat/C/flame_compat_c.hpp \
             src/compat/C/message_board_wrapper.hpp \
             src/compat/C/message_iterator_wrapper.hpp \
             src/mem/memory_manager.hpp \
             src/mem/agent_memory.hpp \
             src/mem/agent_shadow.hpp \
             src/mem/memory_iterator.hpp \
             src/mem/vector_wrapper.hpp \
             src/exe/agent_task.hpp \
             src/exe/fifo_task_queue.hpp \
             src/exe/io_task.hpp \
             src/exe/message_board_task.hpp \
             src/exe/scheduler.hpp \
             src/exe/splitting_fifo_task_queue.hpp \
             src/exe/task_interface.hpp \
             src/exe/task_manager.hpp \
             src/exe/task_queue_interface.hpp \
             src/exe/task_splitter.hpp \
             src/exe/worker_thread.hpp \
             src/mb/board_writer.hpp \
             src/mb/client.hpp \
             src/mb/mb_common.hpp \
             src/mb/message_board_manager.hpp \
             src/mb/message_board.hpp \
             src/mb/message_iterator_backend_raw.hpp \
             src/mb/message_iterator_backend.hpp \
             src/mb/message_iterator.hpp \
             src/mb/message.hpp \
             src/mb/proxy.hpp \
             src/mb/type_validator.hpp \
             headers/exceptions/io.hpp \
             headers/exceptions/base.hpp \
             headers/exceptions/mem.hpp \
             headers/exceptions/exe.hpp \
             headers/exceptions/all.hpp \
             headers/include/flame.h
HEADERS    = $(IO_MODEL_SIM_HEADERS) $(OTHER_HEADERS)
DEPS       = Makefile $(HEADERS)
EXECUTABLE = run_tests
RM         = rm -f

all: $(SOURCES) $(EXECUTABLE)
	
$(EXECUTABLE): $(OBJECTS) 
	$(CPPC) $(LDFLAGS) $(OBJECTS) -o $@ $(LIBS)

$(OBJECTS): $(DEPS)

.cpp.o:
	$(CPPC) -c $(CPPFLAGS) $(INCLUDE) $< -o $@

clean:
	$(RM) $(OBJECTS) $(EXECUTABLE); \
	$(RM) -r cccc

run: all
	./run_tests --log_level=test_suite --catch_system_error=no

doc:
	doxygen

CPPLINT   = /Users/stc/workspace/flame/scripts/cpplint.py
lint:
	$(CPPLINT) $(IO_MODEL_SIM_SRC_TEST) $(IO_MODEL_SIM_HEADERS)

CCCCSUMMARY = /Users/stc/workspace/flame/scripts/ccccsummary.py
cccc:
	cccc $(IO_MODEL_SIM_SRC_TEST) $(IO_MODEL_SIM_HEADERS) --outdir=cccc; \
	$(CCCCSUMMARY) cccc

# dsymutil - only relevent on Mac OS X
valgrind:
	/Users/stc/workspace/valgrind/bin/valgrind --dsymutil=yes --leak-check=full ./$(EXECUTABLE) --log_level=all
	$(RM) -r $(EXECUTABLE).dSYM

.PHONY: cccc

# To run executable:
# export DYLD_LIBRARY_PATH=/Users/stc/workspace/boost/boost_1_48_0/stage/lib