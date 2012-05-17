/*!
 * \file FILENAME
 * \author Shawn Chin
 * \date 2012
 * \copyright Copyright (c) 2012 STFC Rutherford Appleton Laboratory
 * \copyright Copyright (c) 2012 University of Sheffield
 * \copyright GNU Lesser General Public License
 * \brief DESCRIPTION
 */
#ifndef MEM__MEMORY_ITERATOR_HPP_
#define MEM__MEMORY_ITERATOR_HPP_
#include "agent_shadow.hpp"
#include "vector_wrapper.hpp"
#include "exceptions/mem.hpp"

namespace flame { namespace mem {

typedef std::map<std::string, void*> VoidPtrMap;
typedef std::pair<std::string, void*> VoidPtrMapValue;

class MemoryIterator {
  friend class AgentShadow;

  public:
    void Rewind();
    bool Step();
    bool AtEnd() const;
    size_t get_size() const;
    size_t get_position() const;

    template <typename T>
    const T* GetReadPtr(const std::string& var_name) const {
      try {
#ifndef DISABLE_RUNTIME_TYPE_CHECKING
        VectorWrapperBase* vwb = vec_map_ptr_->at(var_name);
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
      if (rw_set_ptr_->find(var_name) == rw_set_ptr_->end()) {
        throw flame::exceptions::invalid_operation("variable is not writeable");
      }

      try {
#ifndef DISABLE_RUNTIME_TYPE_CHECKING
        VectorWrapperBase* vwb = vec_map_ptr_->at(var_name);
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
    explicit MemoryIterator(AgentShadow* shadow);

  private:
    size_t position_;  //! Current iterator position
    size_t size_;
    VoidPtrMap ptr_map_;  //! map of raw pointers of vars
    ConstVectorMap* vec_map_ptr_;  //! pointer to vec map
    WriteableSet* rw_set_ptr_;
};

}}  //  namespace flame::mem


#endif // MEM__MEMORY_ITERATOR_HPP_