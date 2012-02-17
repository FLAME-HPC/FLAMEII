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
#include <boost/static_assert.hpp>
#include <boost/type_traits/is_base_of.hpp>
#include <map>
#include <string>
#include <vector>
#include "agent_memory.hpp"
#include "memory_vector.hpp"

namespace flame { namespace mem {

//! Map to store collection of AgentMemory
typedef std::map<std::string, AgentMemory> AgentMap;

class MemoryManager {
  public:
    MemoryManager() {}
    void RegisterAgent(std::string const& agent_name, size_t pop_size_hint);

    template <typename T>
    void RegisterAgentVar(std::string const& agent_name,
                          std::string const& var_name) {
      AgentMemory &agent = GetAgent(agent_name);
      agent.RegisterVar<T>(var_name);
    }

    template <typename T>
    void RegisterAgentVar(std::string const& agent_name,
                          std::vector<std::string> var_names) {
      AgentMemory &agent = GetAgent(agent_name);

      std::vector<std::string>::iterator it;
      for (it = var_names.begin(); it < var_names.end(); ++it) {
        agent.RegisterVar<T>(*it);
      }
    }

    template <typename T, template <typename> class U>
    U<T> GetVector(std::string const& agent_name, std::string const& var_name) {
      typedef U<T> U_T;
      BOOST_STATIC_ASSERT((boost::is_base_of<impl_::VectorBase, U_T>::value));
      return U_T(GetAgent(agent_name).GetMemoryVector<T>(var_name));
    }

  private:
    AgentMemory& GetAgent(std::string const& agent_name);
    AgentMap agent_map_;

    // disallow copy and assign
    MemoryManager(const MemoryManager&);
    void operator=(const MemoryManager&);

    template <typename T>
    std::vector<T>& GetMemoryVector(std::string const& agent_name,
                                    std::string const& var_name) {
      return GetAgent(agent_name).GetMemoryVector<T>(var_name);
    }

#ifdef TESTBUILD
  // hooks to access private functions from tests
  // TODO: remove once we have a proper method of initialising vectors
  public:
    template <typename T>
    std::vector<T>& GetMemoryVector_test(std::string const& agent_name,
                                         std::string const& var_name) {
      return GetMemoryVector<T>(agent_name, var_name);
    }
#endif

};
}}  // namespace flame::mem
#endif  // MEM__MEMORY_MANAGER_HPP_
