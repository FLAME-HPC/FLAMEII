FLAME-II Documentation {#mainpage}
===============================

[TOC]

Overview  {#mainpage_overview}
========

The FLAME-II package consists of two components -- the FLAME-II headers
and runtime library (libflame2) and a model parser ([xparser](@ref xparser))
which allows modellers to use FLAME-II in a way similar to FLAME-I.

libflame2 consists of the following modules:
 * [MEM](@ref modmem) - Agent memory management
 * [EXE](@ref modexe) - Task scheduling and execution
 * [MB](@ref modmb) - Message management
 * [IO](@ref modio) - Input/Output
 * [MODEL](@ref modmodel) - Model representation 
 * [SIM](@ref modsim) - Simulation management
 * [API](@ref modapi) - Modeller Interface



Building and Installing  {#mainpage_installing}
=======================

To build and install FLAME-II as a user, see 
[Building and Installing FLAME-II](@ref installation).

If you're a FLAME-II developer, see 
[Building FLAME-II for framework development](@ref devbuild).



Using FLAME-II {#mainpage_usage}
==============

In this early release of FLAME-II, we support only the basic usage -- this is modelled 
after FLAME-I to ease transition. See [Basic usage of FLAME-II](@ref usage).

The long term goal for FLAME-II is to serve as a model development framework. While
the basic usage will still be supported in some form, users should also be able to 
define models programmatically and overload framework modules to customise them to 
suit the model, e.g. message boards that uses a custom data structure optimise for
storing and filtering specific message types. This flexibility will enable power users
to build highly parallel and very efficient models without having to start from scratch.

For more details on the design plans, see 
[FLAME-II : a redesign of the flexible large-scale agent-based modelling environment](http://epubs.stfc.ac.uk/bitstream/8576/RAL-TR-2012-019.pdf)
(RAL Technical Reports, RAL-TR-2012-019, November 2012).



Developer Notes {#mainpage_developer}
---------------

For notes on development policies and standards see [Developer Notes](@ref developer).

