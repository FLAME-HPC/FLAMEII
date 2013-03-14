# do these need to be imported?
CC = g++ -g -shared
INCLUDES = $(shell xml2-config --cflags)
LIBS = $(shell xml2-config --libs) -lboost_system-mt -lboost_filesystem-mt

all: io_xml_pop.plugin io_cli_pop.plugin io_csv_pop.plugin io_sqlite_pop.plugin

io_xml_pop.plugin: io_xml_pop.cpp xml_writer.cpp
	$(CC) $(INCLUDES) io_xml_pop.cpp xml_writer.cpp -o io_xml_pop.plugin $(LIBS)

io_cli_pop.plugin: io_cli_pop.cpp
	$(CC) io_cli_pop.cpp -o io_cli_pop.plugin

io_csv_pop.plugin: io_csv_pop.cpp
	$(CC) io_csv_pop.cpp -o io_csv_pop.plugin

io_sqlite_pop.plugin: io_sqlite_pop.cpp
	$(CC) io_sqlite_pop.cpp -o io_sqlite_pop.plugin -lsqlite3

clean:
	rm -f io_xml_pop.plugin io_cli_pop.plugin io_csv_pop.plugin