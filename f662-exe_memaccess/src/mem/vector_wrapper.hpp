/*!
 * \file src/mem/vector_wrapper.hpp
 * \author Shawn Chin
 * \date 2012
 * \copyright Copyright (c) 2012 STFC Rutherford Appleton Laboratory
 * \copyright Copyright (c) 2012 University of Sheffield
 * \copyright GNU Lesser General Public License
 * \brief Proxy classes which provides limited access to memory vectors
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
    virtual void* GetVectorPtr() = 0;
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

    void* GetVectorPtr() {
      return &v_;
    }

    virtual const std::type_info* GetDataType() const {
      return data_type_;
    }

    VectorWrapper<T>* clone() const { return new VectorWrapper<T>(*this); }
  private:
    const std::type_info *data_type_;
    std::vector<T> v_;
};


}}  // namespace flame::mem
#endif  // MEM__MEMORY_VECTOR_HPP
