/*!
 * \file src/mem/memory_manager.hpp
 * \author Shawn Chin
 * \date 2012
 * \copyright Copyright (c) 2012 STFC Rutherford Appleton Laboratory
 * \copyright Copyright (c) 2012 University of Sheffield
 * \copyright GNU Lesser General Public License
 * \brief MemoryManager: management and storage class for agent data
 */
#ifndef MEM__MEMORY_MANAGER_HPP_
#define MEM__MEMORY_MANAGER_HPP_
#include <map>
#include <string>
#include <vector>
#include "agent_memory.hpp"
#include "vector_wrapper.hpp"

namespace flame { namespace mem {

//! Map to store collection of AgentMemory
typedef std::map<std::string, AgentMemory> AgentMap;


//! Memory Manager object.
//! This is a singleton class - only one instance should exist throughtout
//! the simulation. Instances are accessed using MemoryManager::GetInstance().
//! Apart from the Get* methods, all others should be called during the
//! initialisation stage before threads are spawned or guarded by mutexes
class MemoryManager {
  public:
    //! Returns instance of singleton object
    //!  When used in a multithreaded environment, this should be called
    //!  at lease once before threads are spawned.
    static MemoryManager& GetInstance() {
      static MemoryManager instance;
      return instance;
    }

    //! Registers an agent type
    void RegisterAgent(std::string agent_name);

    //! Registers a memory variable of a certain type for a given agent
    template <typename T>
    void RegisterAgentVar(const std::string& agent_name, std::string var_name) {
      GetAgentMemory(agent_name).RegisterVar<T>(var_name);
    }

    //! Registers a list of memory vars or a certain type for a given agent
    template <typename T>
    void RegisterAgentVar(std::string agent_name,
                          const std::vector<std::string>& var_names) {
      AgentMemory& am = GetAgentMemory(agent_name);
      std::vector<std::string>::const_iterator it;
      for (it = var_names.begin(); it != var_names.end(); ++it) {
        am.RegisterVar<T>(*it);
      }
    }

    //! Returns typeless pointer to associated vector wrapper
    VectorWrapperBase* GetVectorWrapper(const std::string& agent_name,
                                        const std::string& var_name);

    //! Returns pointer to std::vector<T> for given agent variable
    template <typename T>
    std::vector<T>* GetVector(const std::string& agent_name,
                              const std::string& var_name) {
      return GetAgentMemory(agent_name).GetVector<T>(var_name);
    }

    //! Provides a hint at the population size of an agent type so memory
    //! utilisation can be optimised
    void HintPopulationSize(const std::string& agent_name,
                            unsigned int size_hint);

    //! Returns the number of registered agents
    size_t GetAgentCount();

    //! Checks if an agent with a given name has been registered
    bool IsRegisteredAgent(const std::string& agent_name);

#ifdef TESTBUILD
    //! Delete all registered agents and vars
    void Reset();
#endif

  private:
    //! This is a singleton class. Disable manual instantiation
    MemoryManager() {}
    //! This is a singleton class. Disable copy constructor
    MemoryManager(const MemoryManager&);
    //! This is a singleton class. Disable assignment operation
    void operator=(const MemoryManager&);

    //! Map used to associate an agent name with an AgentMap object
    AgentMap agent_map_;

    //! Returns an AgentMap object given an agent name
    AgentMemory& GetAgentMemory(const std::string& agent_name);
};
}}  // namespace flame::mem
#endif  // MEM__MEMORY_MANAGER_HPP_
