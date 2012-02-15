#ifndef MEM__AGENT_MEMORY_HPP
#define MEM__AGENT_MEMORY_HPP
#include <map>
#include <vector>
#include <stdexcept>
#include "boost/any.hpp"
// #include "boost/unordered_map.hpp"
#include "boost/shared_ptr.hpp"

namespace flame { namespace mem {

//! Map container used to store memory vectors
typedef std::map<std::string, boost::any> MemoryMap;
// typedef boost::unordered_map<std::string, boost::any> MemoryMap;

//! Key-Value pair for MemoryMap
typedef std::pair<std::string, boost::any> MemoryMapValue;

class AgentMemory
{
  public:
    AgentMemory(std::string agent_name, int pop_size)
        : agent_name_(agent_name),
          population_size_(pop_size) {};

    template <class T>
    void RegisterVar(std::string var_name) {
        typedef std::vector<T> vector_T;
        std::pair<MemoryMap::iterator, bool> ret;

        // Store allocated memory vector in a shared_ptr so deallocation is
        // automatically handled when all all pointers go out of scope
        boost::shared_ptr<vector_T> vec_ptr(new vector_T);
        vec_ptr->reserve(population_size_);

        ret = mem_map_.insert(MemoryMapValue(var_name, vec_ptr));
        if (!ret.second) {  // Ensure that this is an insertion, not replacement
          throw std::domain_error("variable with that name already registered");
        }
    }

    template <class T>
    std::vector<T>& GetMemoryVector(std::string const& var_name) {
        typedef std::vector<T> vector_T;
        typedef boost::shared_ptr<vector_T> ptr_T;

        const MemoryMap::iterator it = mem_map_.find(var_name);
        if (it == mem_map_.end()) {  // key not found
          throw std::out_of_range("Invalid agent memory variable");
        }

        boost::any val = it->second;  // get actual map value
        if (val.type() != typeid(ptr_T)) {
          throw std::domain_error("Invalid type used");
        }

        return *boost::any_cast<ptr_T>(val);
    }

  private:
    std::string agent_name_;
    int population_size_;
    MemoryMap mem_map_;
};
}}  // namespace flame::mem
#endif // MEM__AGENT_MEMORY_HPP
