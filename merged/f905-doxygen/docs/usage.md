Basic Usage of FLAME-II {#usage}
=======================

[TOC]

Building a model {#usage-model}
=============================

You will need access to the FLAME-II executables to parse and compile a model. 
These consists of:

* `xparser` - The parser that you will use to generate the necessary source code and 
  Makefile based on your model definition

* `flame2-config` - Script that provides libflame2 build information. Used by the 
  Makefile to determine the required parameters.

* `flame2-libtool` - Script used by the Makefile to manage the compilation and linking. 
  This customised version of libtool is used to handle discrepancies between GNU libtool 
  and the ones provided by Xcode (for OSX users). 

If you installed FLAME-II into the standard location, these executables should already 
be in your shell search path. If you've installed FLAME-II to a custom location, you'll 
need to make sure that the path is added to `$PATH`. 

    [me@home]$ export PATH="$FLAME2_DIR/bin:$PATH"
    
To check that the executables are accessible, try running `flame2-config --query`. This 
will give you details of your FLAME-II installation.

Parsing a model  {#usage-parsing}
---------------

To parse your model definition and generate the simulation code, run 
`xparser MODELFILE.xml`. Example:

    [me@home]$ xparser model.xml 
    FLAME-II xparser (Version 0.0.1 $Revision: 1137 $)
    --------------------------------------------------
    Reading file: model.xml
    Writing file: agent_function_definitions.hpp
    Writing file: message_datatypes.hpp
    Writing file: flame_api.hpp
    Writing file: main.cpp
    Writing file: Makefile
    Done.

    You can now compile your model using "make". This will compile a debug version
    of the simulation and produce the "run" executable. Always use the debug
    version for model development and testing. This version includes checks to
    detect erroneous operations, and fixes the random seed to aid validation.

    To compile a production version, use "make production". This gives you better
    runtime performance but runs the risk of undefined or incorrect behaviour if
    the model contains erroneous operations.

    Note that you will need to call "make clean" everytime you switch between
    a production and a debug build.
    
Compiling a model {#usage-compiline}
------------------

Once the model is parsed you can compile the generated code by calling `make`. This will
compile a debug version of the simulation and produce the `run` executable.

    [me@home]$ make
    [devel] Compiling artemis_functions.cpp
    [devel] Compiling main.cpp
    [devel] Compiling person_functions.cpp
    Linking executable 'run'

Always use the debug version for model development and testing. This version includes
checks to detect erroneous operations and fixes the random seed to aid validation.

To compile a production version, call `make production`. This gives you better
runtime performance but runs the risk of undefined or incorrect behaviour, or may result
in runtime crashes if the model contains errors.

Note that you will need to call `make clean` every time you switch between production 
and debug builds. THis ensures that every file is recompiled using the correct flags.

Running a simulation {#usage-running}
---------------------

After a successful build, the executable for the model will be created as `run`. 
This executable require 2 input parameters -- the input population data and the 
number of iterations. For example:

    [me@home]$ ./run its/0.xml 2  # run 2 iterations
    Writing file: its/xmlpop.xsd
    Removing file: its/xmlpop.xsd
    Reading file: its/0.xml
    Iteration - 1
    Writing file: its/1.xml
    Iteration - 2
    Writing file: its/2.xml
    Execution time - 0:00:092 [mins:secs:msecs]

The output population data will be written to the same location as the input.

You can also provide an optional third argument which specifies the number of CPU cores 
to use (defaults to a single core). 

    [me@home]$ ./run its/0.xml 2 4  # run 2 iterations across 4 CPU cores
    Writing file: its/xmlpop.xsd
    Removing file: its/xmlpop.xsd
    Reading file: its/0.xml
    Iteration - 1
    Writing file: its/1.xml
    Iteration - 2
    Writing file: its/2.xml
    Execution time - 0:00:070 [mins:secs:msecs]
    

Writing a model (How is it different from FLAME-I?)  {#usage-writing}
===================================================

Changes to the XMML file  {#usage-writing-changes}
------------------------

Model XMML files written for FLAME-I will still parse, however some of the tags 
will be ignore (see section below on features that have not been implemented).

However, function files are expected to be in `*.cpp` files. This may therefore 
require a change in the list of files in `<functionFiles>`. 

### memoryAccess tags

We have introduced a `<memoryAccess>` tag which specifies which memory variables an 
agent funciton will read from and write to. This allows the framework to optimise the 
dependency graph and achieve higher parallelism.

For example, the following states that the `infect` function needs read-only access 
to `x`, `y` and `soul_status` and requires read-write access to `sick_status`. 


    <function>
      <name>infect</name>
      <currentState>2</currentState><nextState>3</nextState>

      <memoryAccess>
        <readOnly>
          <variableName>x</variableName>
          <variableName>y</variableName>
          <variableName>soul_status</variableName>
        </readOnly>

        <readWrite>
          <variableName>sick_status</variableName>
        </readWrite>
      </memoryAccess>
    </function>

Modellers are adviced to limit memory access to only what is require as every memory 
access incurs a small overhead, and more importanly, adds a constraint to the dependency 
graph optimisation thus reducing the potential for parallelism.

Empty tags (`<memoryAccess></memoryAccess>`) signify no memory access is required.

At present, omitting the tags will default to read-write access to all variables. 
This is extremely expensive performance-wise. This may change in the future where tag 
omission will imply no memory access (users will be able to enable compatibility-mode to 
switch to the old defaults). 


Writing agent functions {#usage-writing-functions}
-----------------------

Agent functions are now compiled as C++. Users wishing to stick to C constructs will 
find that nothing much will have changed apart from the APIs for memory and message board 
access.

Changes to make and/or look out for:

* All function files should now include the `flame_api.hpp` header. This header is 
 generated by the xparser and provides acceess to the FLAME-II APIs as well as the 
 model-specific datatypes (e.g. messages).

* Agent transition functions must be defined using the `FLAME_AGENT_FUNCTION()` macro 
 (see example below). This takes care of the input arguments and return types of the 
 function.

* APIs for accessing messages/memory are now different. APIs and macros from FLAME-I 
  will no longer work. Most of the concepts and workflow remain the same, however the 
  syntax will look rather different at first glance.

* Users must be aware that transition functions for different agent instances may be 
  run concurrently. This means being very careful when using external solvers or 
  accessing data beyond what is provided by the framework - watch out for race conditions 
  and conflicting access to data.

* Function files must have the `.cpp` file extension. 

Here's an example of a `function.cpp` file (note the `.cpp` instead of `.c`): 

	#include "flame_api.hpp"

	// Define function "outputdata"
	FLAME_AGENT_FUNCTION(outputdata) {
	  location_message_t msg;  // datatype for holding "location" message
	  msg.x = FLAME.GetMem<double>("x");  // retrieve and assign from memory. Note the datatype in 
	  msg.y = FLAME.GetMem<double>("y");  //   angle brackets which must match the memory datatype 
	  msg.id = FLAME.GetMem<int>("id");   //   specified in the model definition.

	  /* post message with current location */
	  FLAME.PostMessage("location", msg);  // post message to the "location" board.

	  return FLAME_AGENT_ALIVE;
	}

	// Define function "inputdata"
	FLAME_AGENT_FUNCTION(inputdata) {

	  int id = FLAME.GetMem<int>("id");
	  double x = FLAME.GetMem<double>("x");
	  double y = FLAME.GetMem<double>("y");
	  double diameter = FLAME.GetMem<double>("radius") * 2;

	  /* tmp vars */
	  double p, core_distance;
	  double fx = 0, fy = 0;

	  /* loop through messages */
	  MessageIterator iter = FLAME.GetMessageIterator("location");  // get message iterator
	  for (; !iter.AtEnd(); iter.Next()) {  // loop through messages
		location_message_t msg = iter.GetMessage<location_message_t>();  // retrieve message instance
		if (msg.id != id) {
		  core_distance = distance(x, y, msg.x, msg.y);

		  if (core_distance < diameter) {
			p = kr * diameter / core_distance;
			/* accumulate forces */
			fx += (x - msg.x) * p;
			fy += (y - msg.y) * p;
		  }
		}
	  }

	  /* store forces in agent mem */
	  FLAME.SetMem<double>("fx", fx);  // write to memory
	  FLAME.SetMem<double>("fy", fy);

	  return FLAME_AGENT_ALIVE;
	}

See http://ccpforge.cse.rl.ac.uk/svn/flame/models/ for more examples.


FLAME-II User APIs {#usage-api}
==================

All API calls that interact directly with the framework are done via the `FLAME` object. 
For example, `FLAME.SetMem<int>("id", 10);`

Many of the API calls involve a `<...>` construct. This is a C++ feature for 
implementing functions with generic type. This allows us to use a single function to 
handle all dataypes instead of having to define a different function for each type.

Memory access {#usage-api-mem}
-------------

Agent memory can be accessed using `GetMem<T>(VARNAME)` and `SetMem<T>(VARNAME, VALUE)` 
where `T` represents the datatype of the memory variable as defined in the model 
definition. For example: 

    int id = FLAME.GetMem<int>("id");     // returns memory var "id" defined in model.xml with type int

    FLAME.SetMem<double>("y", 0.1);  // sets memory var "y" defined in model.xml with type double
    
If the specified type does not match the model definition, or if an invalid memory access 
is made, the model will compile (because it is still valid code) but an error will be 
raised at run-time and the simulation will terminate.

For performance reasons, **type-checking is only performed in debug builds and not in the 
production builds**. Calling the APIs with mismatching type within a production build 
will result in undefined behaviour -- you may get wrong results, the program may crash, 
or [demons may fly out of your nasal passage](http://catb.org/jargon/html/N/nasal-demons.html). 

**Always develop and test models in debug model and only use production builds 
for validated models.**

For more details, see flame::api::AgentAPI::GetMem and flame::api::AgentAPI::SetMem.

Posting messages {#usage-post-msg}
----------------

To post a message, you first need to create and populate a message instance. 
The datatype to use is `MESSAGENAME_message_t`. This is generated by the parser and is 
essentially a standard C struct with all message vars as struct members. For example, 
to create a "location" message use `location_message_t`. For example:

    location_message_t msg;
    msg.x = FLAME.GetMem<int>("x");
    msg.y = FLAME.GetMem<double>("y");
    msg.id = FLAME.GetMem<int>("id");
 
The message can be then be posted using the `FLAME.PostMessage(MESSAGE_NAME, MESSAGE)`. 

    FLAME.PostMessage("location", msg);
    
The function makes a copy of `msg` so you're free to modify and reuse `msg` for the next 
message.

As with `GetMem` and `SetMem`, type-checking is performed at runtime in debug builds 
and an error is raised if the data passed to `PostMessage` is not of the appropriate 
type. Posting invalid data in production builds may 
[incur the wrath of the nasal demon](http://catb.org/jargon/html/N/nasal-demons.html).

Note that `PostMessage` is also a templated function and defined as 
`PostMessage<T>(string name, T message)`. However, in this case we are allowed to omit 
the `<T>` construct as the datatype of the message can be unambiguously inferred from 
the second argument. See [Omitting Type Identifier](@ref omittype).

For more details, see flame::api::AgentAPI::PostMessage.

Reading messages {#usage-read-msg}
-----------------

To loop through messages, you first need to retrieve an iterator for the message board. 

    MessageIterator iter = FLAME.GetMessageIterator("location");

Iterator provide the following methods:

* `GetCount()` - returns the number of messages accessible via the iterator. This is 
  usually equal to the number of messages in the board unless a filter as been applied. 
  The return type is `size_t`.

* `GetMessage<T>()` - returns the message in the current iteration. The type `T` must be 
  specified and must match the dataype of the message (`MESSAGENAME_message_t`). The 
  return type will be the message datatype.

* `Next()` - step to the next iteration. It returns `true` if the step is successful, or 
 `false` if it fails (already at end of iteration or empty iterator)

* `AtEnd()` - returns `true` if at the end of iteration (or if iterator empty) and 
  `false` otherwise.

* `Rewind()` - rewind to the beginning of the iterator. Does not return anything. 

Here's an example of how you can iterate through messages: 

    MessageIterator iter = FLAME.GetMessageIterator("location");
    for (; !iter.AtEnd(); iter.Next()) {  // loop till iter.AtEnd()
      location_message_t msg = iter.GetMessage<location_message_t>();  // get message
      // use msg here ...
    }


For more details, see flame::api::AgentAPI::GetMessageIterator and 
flame::api::MessageIteratorWrapper.


Customising the build {#usage-custom}
=====================

By default, the generated Makefile will include all headers and sources generated by the 
xparser, as well as source files (for agent functions) specified in the `<functionFiles>` 
field of the model definition.

If you wish to include additional source or header files (e.g. for utility functions 
and solvers), or if you wish to include additional compiler/linker flags, you can create 
a `Makefile.inc` file. This will be included by the Makefile.

The following variables can be defined in `Makefile.inc`:

* `extra_HEADERS` - header files to be considered by Makefile. Changes to these header 
  files will trigger a rebuild when make is called.

* `extra_SOURCES` - source files to add to the compilation

* `extra_COMPILE_FLAGS` - extra flags to pass to the compiler

* `extra_LINK_FLAGS` - extra flags to pass to the linker 

You can define any or all of these variables. You are adviced not to add anything else 
as they may interfere with the main Makefile.

Before adding addition flags, you can check if they are already defined by running 
`flame2-config --cflags` and `flame2-config --libs`.

Here's an example `Makefile.inc` file: 

    # link in extra utility functions and headers that my model can use
    extra_HEADERS = solver.hpp constants.hpp
    extra_SOURCES = solver.cpp
    # our solver needs libgsl
    extra_LINK_FLAGS = -lgsl
    
    
Notes on FLAME-I vs. FLAME-II features {#usage-notes}
=======================================

FLAME-I features that are yet to be implemented:

* State branching
* User defined datatypes
* Message filters
* Message reordering (sort/randomise)
* Environment constants
* Agent creation/deletion
* Distributed memory parallelism (MPI)
* Access to global info such as as iteration count 

FLAME-I features that are likely to be dropped:

* Dynamic arrays in agent memory 

New Features in FLAME-II:

* Concurrent runs of agent functions (Multi-core)
* Memory access specifies
* More robust XML parsing (XMML and pop data) 

New Features planned for FLAME-II:

* Advanced message filtering and indexing
* Customisable message boards
* More efficient I/O formats
* Customisable I/O formats
* Staged checkpointing
* Model profiling (track runtime stats of model to aid optimisation)
* Built-in support for model verification and validation 
