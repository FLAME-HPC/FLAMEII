Module - SIM (Simulation Management) {#modsim}
==================================

[TOC]

Overview {#modsim-overview}
========================

The SIM module handles creating and starting simulations.

Currently the SIM module is make up of the [SimManager](@ref modsim-simmgr) and
[Simulation](@ref modsim-simulation).

`SimManager` provides methods to set global settings:

 * [registerMessageType()](@ref flame::sim::SimManager::registerMessageType) -- 
    registers a message type with the framework.
 * [setPopInputType()](@ref flame::sim::SimManager::setPopInputType) -- 
    set the population data input type.
 * [setPopOutputType()](@ref flame::sim::SimManager::setPopOutputType) -- 
    set the population data output type.

`Simulation` provides methods to create a simulation:

 * [Simulation::Simulation()](@ref flame::sim::Simulation::Simulation) -- 
    create a simulation using the provided Model and initial pop file.
 * [start()](@ref flame::sim::Simulation::start) -- 
    start the simulation using the provided number of iterations and number of cores to use.

Simulation Manager {#modsim-simmgr}
============


Simulation {#modsim-simulation}
============

The initialisation of a [Simulation](@ref flame::sim::Simulation::Simulation):
 * checks the Model has been validated,
 * registers the Model with the MemoryManager,
 * registers the Model with the TaskManager,
 * registers the agent memory information with the IOManager, and
 * reads the initial population data file.

Calling [start](@ref flame::sim::Simulation::start):
 * creates a Scheduler,
 * adds a FIFOTaskQueue to the Scheduler using the provided number of cores to use,
 * assigns all task types to the queue, and
 * calls RunIteration on the Scheduler for the provided number of iterations.