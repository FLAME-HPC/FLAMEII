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
#include <map>
#include <string>
#include <utility>  // for std::pair
#include <vector>
#include <stdexcept>
#include "boost/any.hpp"
// #include "boost/unordered_map.hpp"
// #include "boost/shared_ptr.hpp"

namespace flame { namespace mem {

//! Map container used to store memory vectors
typedef std::map<std::string, boost::any> MemoryMap;
// typedef boost::unordered_map<std::string, boost::any> MemoryMap;

//! Key-Value pair for MemoryMap
typedef std::pair<std::string, boost::any> MemoryMapValue;

class AgentMemory {
  public:
    AgentMemory(std::string agent_name, size_t pop_size_hint)
        : agent_name_(agent_name),
          population_size_(pop_size_hint) {}

    template <typename T>
    void RegisterVar(std::string const& var_name) {
      typedef std::vector<T> vector_T;
      std::pair<MemoryMap::iterator, bool> ret;

      ret = mem_map_.insert(MemoryMapValue(var_name, vector_T()));
      if (!ret.second) {  // Ensure that this is an insertion, not replacement
        throw std::invalid_argument("var with that name already registered");
      }

      // reserve capacity within inserted vector
      vector_T &vec = boost::any_cast<vector_T&>(ret.first->second);
      vec.reserve(population_size_);
    }

    template <typename T>
    std::vector<T>& GetMemoryVector(std::string const& var_name) {
      typedef std::vector<T> vector_T;

      const MemoryMap::iterator it = mem_map_.find(var_name);
      if (it == mem_map_.end()) {  // key not found
        throw std::invalid_argument("Invalid agent memory variable");
      }

      try {
        return boost::any_cast<std::vector<T>&>(it->second);
      }
      catch(boost::bad_any_cast E) {
        throw std::domain_error("Invalid type used");
      }
    }

  private:
    std::string agent_name_;
    int population_size_;
    MemoryMap mem_map_;
};
}}  // namespace flame::mem
#endif  // MEM__AGENT_MEMORY_HPP_
