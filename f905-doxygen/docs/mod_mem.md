Module - MEM (Agent Memory Management) {#modmem}
==================================

[TOC]

The MEM module handles the storage and access to agent memory. All interaction with
the MEM module should be done via the `MemoryManager` -- a singleton instance of
flame::mem::MemoryManager.

Storage {#modmem-storage}
============

The flame::mem::AgentMemory class is used to store the memory of all
instances of agents of a specific type. Each memory variable type is stored in a
vector (one element per agent instance) which, under the hood, is contiguous in memory.

The different memory varaibles of an agent is stored within a
flame::mem::MemoryMap datatype -- a map with the memory variable name as a string-based 
key and a pointer to an instance of flame::mem::VectorWrapper as the values. 
The use of a map allows us to retrieve memory vectors at run-time using a string-based 
lookup key (the variable name).
The use of `VectorWrapper` allows us to store and reference vectors in a 
type-agnostic fashion so in most cases the framework does not need to know the datatype
behind each memory variable thus keeping the APIs clean and simple.


@img{images/memorymap.png, 15cm, MemoryMap - vectors of arbitrary type referenced by a string}

Instead of a standard std::map, `MemoryMap` is defined using boost::ptr_map. 
This allows the lifetime of the `VectorWrapper` objects to be handled automatically by
the containing object, i.e. all vectors are destroyed automatically when the 
`AgentMemory` instance is deleted.

During a simulation, an singleton instance of flame::mem::MemoryManager manages the 
collection of `AgentMemory` instances (one for each agent type). 
References to `AgentMemory` is store in a boost::ptr_map variable much like how 
`MemoryMap` is used but with the agent name as the key and a pointer to the `AgentMemory` 
as the mapped value.

At the framework level, this allows us access to specific memory vectors simply
by querying the `MemoryManager` for the agent name and memory variable name.

    #include "flame2/mem/memory_manager.hpp"
    #include "flame2/mem/vector_wrapper.hpp"

    // Get reference to the MemoryManager (singleton)
    flame::mem::MemoryManager& mgr = flame::mem::MemoryManager::GetInstance();

    // Get pointer to vectorwrapper storing all "x" values for agent "Circle"
    flame::mem::VectorWrapperBase *vw_ptr = mgr.GetVectorWrapper("Circle", "x");


Loading population data {#modmem-loading}
-----------------------

The present scheme for loading agent data is on a per-vector basis. This is reasonably 
efficient but can be error-prone since all vectors within the `AgentMemory` must be
filled to equal size before use.

The loading process starts with the agents and list of variables being registered with
the `MemoryManager`:

    #include "flame2/mem/memory_manager.hpp"

    // Get reference to the MemoryManager (singleton)
    flame::mem::MemoryManager& mgr = flame::mem::MemoryManager::GetInstance();

    // Register agent
    mgr.RegisterAgent("Circle");

    // Register variables (and type) of agent memory
    mgr.RegisterAgentVar<int>("Circle", "id");
    mgr.RegisterAgentVar<double>("Circle", "x");
    mgr.RegisterAgentVar<double>("Circle", "y");
    
Once the variables are registered, the flame::mem::MemoryManager::GetVector or 
flame::mem::MemoryManager::GetVectorWrapper methods can ge used to get a pointer to 
the vector objects for population. It is recommended that the
flame::mem::MemoryManager::HintPopulationSize method is used before data is loaded into
the vectors. This preallocates the memory required by each of the vectors instead of 
allowing them to grow as data is loaded in.

For implementation details, see:
 * flame::mem::VectorWrapper
 * flame::mem::AgentMemory
 * flame::mem::MemoryManager
 
 
Controlling access to agent memory {#modmem-shadow}
===================================

The vectors within `AgentMemory` should never be accessed directly by users. To control
access to agent memory, a *shadow* of the agent (flame::mem::AgentShadow) should be
created for each task that needs access to agent memory -- the *shadow* provides a view 
of the agent with predefined access permissions (read/write to a subset of variables). 
It also provides a means to iterate through agent memory in a column-wise fashion 
(one agent instance at a time).

An `AgentShadow` should only be instantiated via the `MemoryManager` using
flame::mem::MemoryManager::GetAgentShadow. This returns a `boost::shared_ptr` to a new 
`AgentShadow` instance. (Using a shared_ptr means that the object is automatically 
destructed once all pointers to it go out of scope.)

Access to agent memory variables is registered using flame::mem::AgentShadow::AllowAccess.
When a variable is registered, a pointer to the corresponding `VectorWrapperBase` is 
cached to reduce the level of indirection for each access. The *shadow* will only allow
access to variables that are registered, and the default access mode is read-only.
Write-access must be explicitly requested via the optional argument 
to `AgentShadow::AllowAccess()`.

    #include "flame2/mem/memory_manager.hpp"

    // Get reference to the MemoryManager (singleton)
    flame::mem::MemoryManager& mgr = flame::mem::MemoryManager::GetInstance();
    
    // Create a shadow of the "Circle" agent
    flame::mem::AgentShadowPtr shadow = mgr.GetAgentShadow("Circle");
    // Allow read access to "x", "y" and write access to "force"
    shadow->AllowAccess("x");
    shadow->AllowAccess("y");
    shadow->AllowAccess("force", true);  // RW access

To iterate through agent memory, a flame::mem::MemoryIterator is instantiated by calling
`shadow->GetMemoryIterator()`. 

The `MemoryIterator` instance will contain a 
`std::map` of `void` pointers which references the raw value within the vectors.
At the beginning of an iteration, the pointers will point to the 0-th element of all 
associated vectors. When `MemoryIterator.Step()` is called, these pointers are 
incremented using `VectorWrapperBase->StepRawPtr(ptr)` which is a virtual function 
that will call the overloaded implementation for each type (and so the pointers are 
incremented according to the correct type size).

During each iteration, actual memory values can be accessed via the `MemoryIterator`
using `Get<T>(var_name)` and updated using `Set<T>(var_name, value)`. 
Accessing a variable that has not been given access to or writing to a non-writeable 
variable will raise flame::exception::invalid_operation when run in debug mode. 
The `AtEnd()` method can be used to check for end of iteration. 

For example, to iterate through agent memory using the *shadow* instances created above:

    // retrieve an iterator to read/write agent data
    flame::mem::MemoryIteratorPtr iter = shadow->GetMemoryIterator();

    // Use .AtEnd() and .Step() to iterator through data
    for (; !iter->AtEnd(); iter->Step()) {
      double x = iter->Get<double>("x");  // Note: type required
      double y = iter->Get<double>("y");
      // ... 
      iter->Set<double>("force", 100);  // we have write access to "force"
    }

In a simulation setup, `AgentShadow` instances would be attached to each 
flame::exe::AgentTask and managed via flame::exe::AgentTask::AllowAccess. This would be
done based on the memory access requirements of the specific task as defined by the model.
During the execution of the `AgentTask`, a `MemoryIterator` is derived and iterated. The 
flame API used by modellers (flame::api::AgentAPI) stores a private reference to the
memory iterator and provides restricted access to the iterator via its `GetMem<T>()` and 
`SetMem<T>()` methods.

For implementation details, see:
 * flame::mem::AgentShadow
 * flame::mem::MemoryIterator
 * flame::exe::AgentTask
 * flame::api::AgentAPI
 