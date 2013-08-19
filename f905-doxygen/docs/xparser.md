XParser  {#xparser}
=======

[TOC]

Overview {#xparser-overview}
========================

[Xparser](@ref xparser2.cpp) is a program that takes a model XML file and writes a simulation program that uses FLAME-II.

Basic usage of `xparser` is explained in [Basic Usage of FLAME-II](@ref usage).

Files Generated
===============

Xparser generates the following files:

 * `agent_function_definitions.hpp` -- 
    header file for agent function definitions.
 * `message_datatypes.hpp` -- 
    define message types.
 * `flame_api.hpp` -- 
    umbrella header file which all model function files should include.
 * `main.cpp` -- 
    simulation program containing main function.
 * `Makefile` -- 
    compilation configuration.

Via command line options Xparser can generate state and dependency graphs of models:

 * `stategraph.dot` -- state graph generated using `--state_graph` or `-s`.
 * `dependencygraph.dot` -- dependency graph generated using `--dependency_graph` or `-d`.

These are dot style graphs and require [Graphviz](http://www.graphviz.org/) to view.

Constructing the Simulation
============

How xparser creates a simulation can be the basis for creating a model directly in code 
rather than via an XML file.

A FLAME-II model object is created, agents and messages are added, and 
the model is validated:

    flame::model::Model model;
    model.addAgent("Agent");
    model.addAgentVariable("Agent", "int", "id");
    flame::model::AgentFunction Function_1("f1", "start", "end");
    Function_1.addOutput("message");
    Function_1.addReadOnlyVariable("id");
    model.addAgentFunction("Agent", Function_1);
    model.addMessage("message");
    model.addMessageVariable("message", "int", "id");
    model.validate();

The model requires the function pointer related to any declared agent functions:

    model.registerAgentFunction("f1", &f1);

A simulation manager object is created, simulation population input and output 
types are set and message types associated with messages are registered.

    flame::sim::SimManager sim_mgr;
    sim_mgr.setPopInputType("xml");
    sim_mgr.setPopOutputType("xml");
    sim_mgr.registerMessageType<message_message_t>("message");

A simulation object can then be created and started:

    flame::sim::Simulation s(model, "its/0.xml");
    size_t num_iters = 10;
    size_t num_cores = 1;
    s.start(num_iters, num_cores);

