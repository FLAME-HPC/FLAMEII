/*!
 * \file src/mem/agent_memory.hpp
 * \author Shawn Chin
 * \date 2012
 * \copyright Copyright (c) 2012 STFC Rutherford Appleton Laboratory
 * \copyright Copyright (c) 2012 University of Sheffield
 * \copyright GNU Lesser General Public License
 * \brief AgentMemory: management and storage class for per-agent memory vectors
 */
#ifndef MEM__AGENT_MEMORY_HPP_
#define MEM__AGENT_MEMORY_HPP_
//#include <map>
#include <string>
#include <utility>  // for std::pair
#include <vector>
#include <stdexcept>
#include "boost/ptr_container/ptr_map.hpp"
// #include "boost/any.hpp"
// #include "boost/unordered_map.hpp"
// #include "boost/shared_ptr.hpp"
#include "vector_wrapper.hpp"

namespace flame { namespace mem {

//! Map container used to store memory vectors
typedef boost::ptr_map<std::string, VectorWrapperBase> MemoryMap;

//! Key-Value pair for MemoryMap
//typedef std::pair<std::string, boost::any> MemoryMapValue;

class AgentMemory {
  public:
    AgentMemory(std::string agent_name)
        : agent_name_(agent_name),
          registration_closed_(false) {}

    //! Register a memory variable
    template <typename T>
    void RegisterVar(std::string var_name) {
      if (registration_closed_) {
        throw std::runtime_error("variables can no longer be registered");
      }
      std::pair<MemoryMap::iterator, bool> ret;
      ret = mem_map_.insert(var_name, new VectorWrapper<T>());
      if (!ret.second) {  // if replacement instead of insertion
        throw std::invalid_argument("variable already registered");
      }
    }

    //! Hint at a population size so required memory can be reserved
    //! in advance. This saves having to constantly reallocate memory
    //! as agents are added to AgentMemory.
    void HintPopulationSize(unsigned int size_hint);

  private:
    std::string agent_name_;
    MemoryMap mem_map_;

    //! Indicates that vectors have been resized/populated so new variables
    //!  should no longer be registered.
    bool registration_closed_;
};
}}  // namespace flame::mem
#endif  // MEM__AGENT_MEMORY_HPP_
