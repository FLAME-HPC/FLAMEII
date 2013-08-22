Module - IO (Input/Output) {#modio}
==================================

[TOC]

Overview {#modexe-overview}
========================

The IO module handles the reading of models and reading and writing population data.

Most interaction with the IO module would be done via the methods provided by the 
[IOManager](@ref modio-iomgr).

Read and writing of pop files are handled via the [IOInterface](@ref modio-iointerface).

[IO Plugins](@ref modio-ioplugins) subclass the `IOInterface` and provide implementations
for different file formats.

IO Manager {#modio-iomgr}
============

The [IOManager](@ref flame::io::IOManager) is a singleton object that handles 
the reading of a model and reading and writing population data.

The most common `IOManager` methods are:

 * [IOManager::GetInstance()](@ref flame::io::IOManager::GetInstance) -- 
   class method to returns a reference to the singleton
   `IOManager` instance. All methods should be run off this instance. 
 * [loadModel()](@ref flame::io::IOManager::loadModel) -- 
    loads a model from a given file (called by Model when given a file).
 * [setInputType()](@ref flame::io::IOManager::setInputType) -- 
    Set the population input type.
 * [setOutputType()](@ref flame::io::IOManager::setOutputType) -- 
    Set the population output type.
 * [readPop()](@ref flame::io::IOManager::readPop) -- 
    reads a population file.
 * [initialiseData()](@ref flame::io::IOManager::initialiseData) -- 
    initialise writing of pop data (called by io task).
 * [writePop()](@ref flame::io::IOManager::writePop) -- 
    write an agent variable array (called by io task).
 * [finaliseData()](@ref flame::io::IOManager::finaliseData) -- 
    finalise writing of pop data (called by io task).

IO Interface {#modio-iointerface}
============

The [IOInterface](@ref flame::io::IOInterface) is an abstract interface for implementing IO plugins.
The interface provides the following concrete methods:

 * [setAgentMemoryInfo](@ref flame::io::IOInterface::setAgentMemoryInfo) --
    called by `IOManger` to set agent memory information.
 * [setIteration](@ref flame::io::IOInterface::setIteration) --
    updated by `IOManager` when reading an initial pop file and by the `Scheduler` 
    [RunIteration](@ref flame::exe::Scheduler::RunIteration) method.
 * [setPath](@ref flame::io::IOInterface::setPath) --
    called by `IOManager` to define where to write pop files to.
 * [getVariableType](@ref flame::io::IOInterface::getVariableType) --
    a private method to provide the variable type of agent variables.

The interface provides the following pure virtual methods that need overriding:

 * [getName](@ref flame::io::IOInterface::getName) --
    returns the name of the plugin.
 * [readPop](@ref flame::io::IOInterface::readPop) --
    reads the pop data file provided.
 * [initialiseData](@ref flame::io::IOInterface::initialiseData) --
    initialises any pop data file.
 * [writePop](@ref flame::io::IOInterface::writePop) --
    writes out an agent variable for all agents.
 * [finaliseData](@ref flame::io::IOInterface::finaliseData) --
    finalises any pop data file.

The method [readPop](@ref flame::io::IOInterface::readPop) currently provides two function pointers:

 * [addInt](@ref flame::io::addInt) -- to add integers to agent variable arrays.
 * [addDouble](@ref flame::io::addDouble) -- to add doubles to agent variable arrays.

IO Plugins {#modio-ioplugins}
============

The framework provides some built-in IO plugins:

 * [XML](@ref flame::io::IOXMLPop) -- 
   the default, but is not column based and writes all data out on finalising the data.
 * [CSV](@ref flame::io::IOCSVPop) -- 
   again not column based and writes all data out on finalising the data.
 * [CLI](@ref flame::io::IOCLIPop) -- 
   no reading option but writes output to the command line to help debugging.
 * [HDF5](@ref flame::io::IOHDF5Pop) -- 
   compile option, currently the 'best' output format, make default?
 * [SQLite](@ref flame::io::IOSQLitePop) -- 
   compile option, a database but using indexes to write column wise.

For speedup FLAME-II tries to write out agent data vector wise per agent variable.
This requires a data format that can write to a file column wise.
Unfortunately most file formats expect data to be row wise.
For example XML and CSV can only, realistically, be written line by line, agent by agent, and not variable by variable.
SQLite uses a database which also expects data to be added in rows. We can circumvent this by indexing rows and writing
each variable row by row which isn't perfect but works, especially if we batch the SQL statements to be executed.
HDF5 on the other hand can write vectors very efficiently and is a much better match for FLAME-II requirements.

The problem with writing agent variables vector wise is at the end of each iteration when agents can be
removed and added to a simulation.
Removing elements from vectors requires packing each of the variable vectors which is time consuming and counterproductive.
A solution which be to provide an index to removed agents but this is also an inefficient overhead.

Note: a way for users to plugin there own IO plugins at runtime was developed but was found to be platform dependent.
There has been no further development of this feature.
