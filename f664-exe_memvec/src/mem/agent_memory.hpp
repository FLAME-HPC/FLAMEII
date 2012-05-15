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
#include <string>
#include <utility>  // for std::pair
#include <vector>
#include <typeinfo>
#include "boost/ptr_container/ptr_map.hpp"
#include "exceptions/mem.hpp"
#include "vector_wrapper.hpp"

namespace flame { namespace mem {

namespace exc = flame::exceptions;

//! Map container used to store memory vectors
typedef boost::ptr_map<std::string, VectorWrapperBase> MemoryMap;

//! Container for memory vectors associated with an agent type
class AgentMemory {
  public:
    explicit AgentMemory(const std::string& agent_name)
        : agent_name_(agent_name),
          registration_closed_(false) {}

    //! Registers a memory variable of a specific type
    template <typename T>
    void RegisterVar(std::string var_name) {
      if (registration_closed_) {
        throw exc::logic_error("variables can no longer be registered");
      }
      std::pair<MemoryMap::iterator, bool> ret;
      ret = mem_map_.insert(var_name, new VectorWrapper<T>());
      if (!ret.second) {  // if replacement instead of insertion
        throw exc::logic_error("variable already registered");
      }
    }

    //! Hint at a population size so required memory can be reserved
    //! in advance. This saves having to constantly reallocate memory
    //! as agents are added to AgentMemory.
    void HintPopulationSize(unsigned int size_hint);

    //! Returns typeless pointer to associated vector wrapper
    VectorWrapperBase* GetVectorWrapper(const std::string& var_name);

    //! Returns a pointer to the actual data vector
    template <typename T>
    std::vector<T>* GetVector(const std::string& var_name) {
      VectorWrapperBase* ptr;
      try {
        ptr = &(mem_map_.at(var_name));
      }
      catch(const boost::bad_ptr_container_operation& E) {
        throw exc::invalid_variable("Invalid agent memory variable");
      }

#ifndef DISABLE_RUNTIME_TYPE_CHECKING
      if (*(ptr->GetDataType()) != typeid(T)) {
        throw exc::invalid_type("Invalid data type specified");
      }
#endif
      return static_cast<std::vector<T>*>(ptr->GetVectorPtr());
    }

  private:
    std::string agent_name_;
    MemoryMap mem_map_;

    //! Indicates that vectors have been resized/populated so new variables
    //!  should no longer be registered.
    bool registration_closed_;
};
}}  // namespace flame::mem
#endif  // MEM__AGENT_MEMORY_HPP_
