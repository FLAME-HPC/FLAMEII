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
    typedef T data_type;
    typedef std::vector<T> vector_type;
    void reserve(unsigned int n) { v_.reserve(n); }
    VectorWrapper<T>* clone() const { return new VectorWrapper<T>(*this); }
  private:
    std::vector<T> v_;
};


}}  // namespace flame::mem
#endif  // MEM__MEMORY_VECTOR_HPP
