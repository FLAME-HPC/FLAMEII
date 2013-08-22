Module - API (Modeller Interface) {#modapi}
==================================

[TOC]

Overview {#modapi-overview}
=========

The API module provides the methods used in agent transition functions to
interact with the framework. This includes accesses to agent memory, message
boards, environment constants (to be implemented), etc.

The following APIs are currently supported:
 * [GetMem](@ref flame::api::AgentAPI::GetMem)
 * [SetMem](@ref flame::api::AgentAPI::SetMem) 
 * [PostMessage](@ref flame::api::AgentAPI::PostMessage)
 * [GetMessageIterator](@ref flame::api::AgentAPI::GetMessageIterator)
 
For an example of how the API is used, see
[Writing agent functions](@ref usage-writing-functions).


The AgentAPI class {#modapi-agentapi}
========================

Every agent function -- defined using the
[FLAME_AGENT_FUNCTION](@ref FLAME_AGENT_FUNCTION) macro -- will
have access to a  `FLAME` variable which is a reference to the an instance
of [AgentAPI](@ref flame::api::AgentAPI). This instance acts
as the main access point for agent functions to call framework routines.
The instance passed to each function will be unique to it and will
encapsulate access control information such as readable/writeable memory
variables, message board accesses, etc.

Access control is achieved by storing a shared pointers to
flame::mem::MemoryIterator and flame::mb::Client instances. These instances
are assigned by the flame::exe::AgentTask before the functions are
executed. See [Controlling access to agent memory](@ref modmem-shadow)
and [API Access to Message Boards](@ref modmb-api-acl) for more details.

In addition to redirecting calls to the appropriate framework routines,
the API methods also traps all exceptions an reraises them using an
appropriate subclass of flame::exceptions::flame_api_exception along with
a more detailed error message.

For implementation details, see:
 * flame::api::AgentAPI
 * [FLAME_AGENT_FUNCTION](@ref FLAME_AGENT_FUNCTION)
 * flame::exe::AgentTask::Run

MessageIterator Wrapper {#modapi-iterwrapper}
=========================

When an agent function calls `FLAME->GetMessageIterator()`, a
[MessageIteratorWrapper](@ref flame::api::MessageIteratorWrapper) is
returned instead of a [MessageIterator](@ref flame::mb::MessageIterator).

This was introduced to enable a cleaner and consistent API -- it allows
users to access iterator methods directly rather than using the `->`
indirection.

While not technically useful, we also define a default constructor so users
can declare an iterator instance without assigning it. This reduces the
number of gotchas in the API. E.g.

    MessageIterator iter;  // declare obj without assignment (null ptr)
    // ...
    iter = FLAME.GetMessageIterator("message_name");  // assign later


When the default constructor is used, a null shared pointer is created.
Dereferencing this will cause an assertion error. It is therefore the users'
responsibility to ensure that instances should not be used until assigned
a value using the GetMessageIterator() API call.

Checking the pointer each time a method is called may be expensive in when
done repeatedly in a tight loop. To avoid this overhead, the checks are
compiled away in the production version of the library (users are expected
to use the debug version for development work).

For implementation details, see:
 * flame::api::AgentAPI
 * flame::api::MessageIteratorWrapper
