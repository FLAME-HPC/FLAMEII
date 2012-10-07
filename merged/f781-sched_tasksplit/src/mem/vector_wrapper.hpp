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
#include <typeinfo>
#include "boost/any.hpp"
#include "exceptions/all.hpp"

namespace flame { namespace mem {

//! Base for vector classes that can be used as proxy object for
//! agent memory vectors.
class VectorWrapperBase {
  public:
    virtual ~VectorWrapperBase() {}
    virtual void reserve(unsigned int n) = 0;
    virtual size_t size() const = 0;
    virtual bool empty() const = 0;
    virtual void clear() = 0;

    virtual void* GetVectorPtr() = 0;

    //! Append contents of vec into the internal vector
    virtual void Extend(VectorWrapperBase* vec) = 0;

    //! Return a boost::any object of current type initialsed with value
    //! pointed to by raw pointer.
    virtual boost::any ConvertToBoostAny(void* ptr) = 0;

    //! Returns a pointer to the Nth element in the internal
    //! array, or NULL if the vector is empty
    virtual void* GetRawPtr(size_t offset = 0) = 0;

    //! Takes a raw pointer to the internal array element and
    //! returns a pointer to the next next element in the array
    //! or NULL if the end of the array is reached
    virtual void* StepRawPtr(void* ptr) = 0;

    //! Appends to vector using value wrapped in boost::any
    virtual void push_back(boost::any value) = 0;

    virtual const std::type_info* GetDataType() const = 0;

    virtual VectorWrapperBase* clone_empty() const = 0;

    //! Simulate a virtual copy constructor
    virtual VectorWrapperBase* clone() const = 0;
};

//! Make VectorWrapperBase cloneable within boost::ptr_map
inline VectorWrapperBase* new_clone(const VectorWrapperBase& a) {
  return a.clone();
}

//! Type specific VectorWrappers
template <typename T>
class VectorWrapper: public VectorWrapperBase {
  public:
    typedef T data_type;
    typedef std::vector<T> vector_type;

    VectorWrapper() { data_type_ = &typeid(T); }

    explicit VectorWrapper(const VectorWrapper& v) {
      data_type_ = v.data_type_;
      v_ = v.v_;
    }

    void reserve(unsigned int n) { v_.reserve(n); }
    size_t size() const { return v_.size(); }
    bool empty() const { return v_.empty(); }
    void clear() { v_.clear(); }

    void Extend(VectorWrapperBase* vec) {
      if (GetDataType() != vec->GetDataType()) {
        throw flame::exceptions::invalid_type("mismatching type");
      }
      vector_type *content = static_cast<vector_type*>(vec->GetVectorPtr());
      v_.insert(v_.end(), content->begin(), content->end());
    }

    void* GetVectorPtr() {
      return &v_;
    }

    void* GetRawPtr(size_t offset) {
      if (offset == 0) {
        return (v_.empty()) ? NULL : &(v_.front());
      } else {
        if (offset >= v_.size()) {
          throw flame::exceptions::invalid_argument("invalid offset");
        }
        return &v_[offset];
      }
    }

    boost::any ConvertToBoostAny(void* ptr) {
      return boost::any(*(static_cast<T*>(ptr)));
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

    //! Appends to vector using value wrapped in boost::any
    void push_back(boost::any value) {
      try {
        v_.push_back(boost::any_cast<T>(value));
      } catch(const boost::bad_any_cast& E) {
        throw flame::exceptions::invalid_type("mismatching type");
      }
    }

    VectorWrapper<T>* clone_empty() const { return new VectorWrapper<T>(); }

    VectorWrapper<T>* clone() const { return new VectorWrapper<T>(*this); }

  private:
    const std::type_info *data_type_;
    std::vector<T> v_;
};


}}  // namespace flame::mem
#endif  // MEM__MEMORY_VECTOR_HPP
