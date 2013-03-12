CC = g++ -g -shared
# need first include for "flame2/config.hpp" which shouldn't be needed
INCLUDES = $(shell xml2-config --cflags) -I../../../
LIBS = $(shell xml2-config --libs) -lboost_system-mt -lboost_filesystem-mt

all: io_xml_pop.plugin io_cli_pop.plugin

io_xml_pop.plugin: io_xml_pop.cpp xml_writer.cpp
	$(CC) $(INCLUDES) io_xml_pop.cpp xml_writer.cpp -o io_xml_pop.plugin $(LIBS)

io_cli_pop.plugin: io_cli_pop.cpp
	$(CC) -I../../../ io_cli_pop.cpp -o io_cli_pop.plugin

clean:
	rm io_xml_pop.plugin io_cli_pop.plugin