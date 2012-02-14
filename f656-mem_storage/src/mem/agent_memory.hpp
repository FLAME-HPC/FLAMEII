#ifndef MEM__AGENT_MEMORY_HPP
#define MEM__AGENT_MEMORY_HPP
#include <vector>
#include <cassert>
#include "boost/any.hpp"
#include "boost/unordered_map.hpp"
#include "boost/shared_ptr.hpp"

namespace flame { namespace mem {

class AgentMemory
{
  public:
    AgentMemory(std::string agent_name, int pop_size)
        : agent_name_(agent_name),
          population_size_(pop_size) {};

    template <class T>
    void RegisterVar(std::string var_name) {
        typedef std::vector<T> vector_T;
        std::pair<vector_T*, bool> res;
        boost::shared_ptr<vector_T> vec_ptr(new vector_T);
        vec_ptr->reserve(population_size_);
        mem_map_[var_name] = vec_ptr;
    }

    template <class T>
    std::vector<T>& GetMemoryVector(std::string var_name) {
        typedef std::vector<T> vector_T;
        typedef boost::shared_ptr<vector_T> ptr_T;
        try {
            boost::any &v = mem_map_.at(var_name);
            if (v.type() != typeid(ptr_T)) {
                throw std::domain_error("Invalid type used");
            }
            return *boost::any_cast<ptr_T>(v);
        }
        catch(std::out_of_range E) {
            throw std::out_of_range("Invalid agent memory variable");
        }
    }

  private:
    std::string agent_name_;
    int population_size_;
    boost::unordered_map<std::string, boost::any> mem_map_;
};
}}  // namespace flame::mem
#endif // MEM__AGENT_MEMORY_HPP
