/*!
 * \file FILENAME
 * \author Shawn Chin
 * \date 2012
 * \copyright Copyright (c) 2012 STFC Rutherford Appleton Laboratory
 * \copyright Copyright (c) 2012 University of Sheffield
 * \copyright GNU Lesser General Public License
 * \brief DESCRIPTION
 */
#ifndef MEM__MEMORY_VECTOR_HPP
#define MEM__MEMORY_VECTOR_HPP
#include <vector>

namespace flame { namespace mem {

//! Base for vector classes that can be used as proxy object for
//! agent memory vectors.
class VectorWrapperBase {
  public:
    virtual ~VectorWrapperBase() {}
    virtual void reserve(unsigned int n) = 0;
    virtual size_t size() const = 0;
    virtual bool empty() const = 0;

    virtual void* GetVectorPtr() = 0;

    //! Returns a pointer to the first element in the internal
    //! array, or NULL if the vector is empty
    virtual void* GetRawPtr() = 0;

    //! Takes a raw pointer to the internal array element and
    //! returns a pointer to the next next element in the array
    //! or NULL if the end of the array is reached
    virtual void* StepRawPtr(void* ptr) = 0;

    virtual const std::type_info* GetDataType() const = 0;

    //! Simulate a virtual copy constructor
    virtual VectorWrapperBase* clone() const = 0;
};

//! Make VectorWrapperBase cloneable within boost::ptr_map
inline VectorWrapperBase* new_clone(const VectorWrapperBase& a) {
  return a.clone();
}

template <typename T>
class VectorWrapper: public VectorWrapperBase {
  public:
    VectorWrapper() { data_type_ = &typeid(T); }
    typedef T data_type;
    typedef std::vector<T> vector_type;
    void reserve(unsigned int n) { v_.reserve(n); }
    size_t size() const { return v_.size(); }
    virtual bool empty() const { return v_.empty(); }

    void* GetVectorPtr() {
      return &v_;
    }

    void* GetRawPtr() {
      return (v_.empty()) ? NULL : &(v_.front());
    }

    void* StepRawPtr(void* ptr) {
      if (ptr == NULL || ptr == &(v_.back())) {
        return NULL;
      }
      return static_cast<void*>(static_cast<T*>(ptr)+1);
    }

    const std::type_info* GetDataType() const {
      return data_type_;
    }

    VectorWrapper<T>* clone() const { return new VectorWrapper<T>(*this); }

  private:
    const std::type_info *data_type_;
    std::vector<T> v_;
};


}}  // namespace flame::mem
#endif  // MEM__MEMORY_VECTOR_HPP
