/*!
 * \file src/mem/agent_memory_iterator.hpp
 * \author Shawn Chin
 * \date 2012
 * \copyright Copyright (c) 2012 STFC Rutherford Appleton Laboratory
 * \copyright Copyright (c) 2012 University of Sheffield
 * \copyright GNU Lesser General Public License
 * \brief Iterator to step through AgentMemory by agent (col-wise)
 */
//! TODO(lsc): since the number of entries in vec_map_ and rw_set_ will be
//! relatively small, the tree-based search used by std::map and
//! std::set may not be ideal. Consider implementing variants that
//! uses a sorted vector instead. See:
//!  - AssocVector: http://loki-lib.sourceforge.net/html/a00645.html
//!  - http://lafstern.org/matt/col1.pdf
//!  - http://www.codeproject.com/Articles/27799/Stree

#ifndef MEM__AGENT_MEMORY_ITERATOR_HPP_
#define MEM__AGENT_MEMORY_ITERATOR_HPP_
#include <map>
#include <set>
#include <vector>
#include <string>
#include <utility>
#include <stdexcept>
#include "agent_memory.hpp"
#include "exceptions/all.hpp"

namespace flame { namespace mem {

typedef std::map<std::string, VectorWrapperBase* const> ConstVectorMap;
typedef std::pair<std::string, VectorWrapperBase* const> ConstVectorMapValue;
typedef std::map<std::string, void*> VoidPtrMap;
typedef std::pair<std::string, void*> VoidPtrMapValue;

class AgentMemoryIterator {
  friend class MemoryManager;
  public:
    void AllowAccess(const std::string& var_name, bool writeable = false);
    // TODO(lsc): void AllowFullAccess(bool writeable = false);
    void Rewind();
    bool Step();
    bool AtEnd() const;
    size_t get_size() const;
    size_t get_position() const;

    template <typename T>
    const T* GetReadPtr(const std::string& var_name) const {
      try {
#ifndef DISABLE_RUNTIME_TYPE_CHECKING
        VectorWrapperBase* vwb = vec_map_.at(var_name);
        if (*(vwb->GetDataType()) != typeid(T)) {
          throw flame::exceptions::invalid_type("invalid type");
        }
#endif
        return static_cast<const T* const>(ptr_map_.at(var_name));
      }
      catch(const std::out_of_range& E) {
        throw flame::exceptions::invalid_variable("invalid variable");
      }
    }

    template <typename T>
    T* GetWritePtr(const std::string& var_name) const {
      if (rw_set_.find(var_name) == rw_set_.end()) {
        throw flame::exceptions::invalid_operation("variable is not writeable");
      }

      try {
#ifndef DISABLE_RUNTIME_TYPE_CHECKING
        VectorWrapperBase* vwb = vec_map_.at(var_name);
        if (*(vwb->GetDataType()) != typeid(T)) {
          throw flame::exceptions::invalid_type("invalid type");
        }
#endif
        return static_cast<T* const>(ptr_map_.at(var_name));
      }
      catch(const std::out_of_range& E) {
        throw flame::exceptions::invalid_variable("invalid variable");
      }
    }

    template <typename T>
    T Get(const std::string& var_name) const {
      const T* ptr = GetReadPtr<T>(var_name);
      if (ptr == NULL) {
        throw flame::exceptions::out_of_range("end of iterator met");
      } else {
        return *(ptr);
      }
    }

    template <typename T>
    void Set(const std::string& var_name, T value) {
      T* ptr = GetWritePtr<T>(var_name);
      if (ptr == NULL) {
        throw flame::exceptions::out_of_range("end of iterator met");
      } else {
        *(ptr) = value;
      }
    }

  protected:
    explicit AgentMemoryIterator(AgentMemory* am);

  private:
    size_t size_;  //! Size if memory vectors
    size_t position_;  //! Current iterator position

    AgentMemory* am_;
    std::set<std::string> rw_set_;  //! Set of var names that has write access
    ConstVectorMap vec_map_;  //! map accessible vars
    VoidPtrMap ptr_map_;  //! map of raw pointers of vars

    AgentMemoryIterator(const AgentMemoryIterator&);  //! Disable copy ctor
    void operator=(const AgentMemoryIterator&);  //! Disable assignment
};

}}  //  namespace flame::mem
#endif  // MEM__AGENT_MEMORY_ITERATOR_HPP_
