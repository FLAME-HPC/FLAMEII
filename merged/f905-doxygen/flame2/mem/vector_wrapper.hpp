/*!
 * \file flame2/mem/vector_wrapper.hpp
 * \author Shawn Chin
 * \date 2012
 * \copyright Copyright (c) 2012 STFC Rutherford Appleton Laboratory
 * \copyright Copyright (c) 2012 University of Sheffield
 * \copyright GNU Lesser General Public License
 * \brief Wrapper for vectors to allow type-agnostic access and storage
 */
#ifndef MEM__MEMORY_VECTOR_HPP
#define MEM__MEMORY_VECTOR_HPP
#include <string>
#include <vector>
#include <ostream>
#include <typeinfo>
#include <boost/any.hpp>
#include "flame2/exceptions/all.hpp"

namespace flame { namespace mem {

//! \brief Abstract base class of a proxy object that can be used to refer to
//!        vectors of arbitrary type.
//!
//! Templated derived classes of this class form the main data storage for
//! agent memory and the default message board.
//!
//! This class allows us to store, refer to, and operate on vectors in a
//! type-agnostic fashion.
class VectorWrapperBase {
  public:
    virtual ~VectorWrapperBase() {}

    //! \brief Request a change in capacity
    //!
    //! \param n minimum capacity required for the vector
    //!
    //! This calls std::vector::reserve on the underlying vector object.
    virtual void reserve(unsigned int n) = 0;

    //! \brief Returns the number of elements in the vector
    //! \returns number of elements in the vector
    //!
    //! This calls std::vector::size on the underlying vector object.
    virtual size_t size() const = 0;

    //! \brief Returns whether the vector is empty (i.e. whether its size is 0)
    //! \returns true if vector is empty, false otherwise
    //!
    //! This calls std::vector::empty on the underlying vector object.
    virtual bool empty() const = 0;

    //! \brief Removes all elements from the vector leaving the container
    //!        with size 0
    //!
    //! This calls std::vector::clear on the underlying vector object.
    virtual void clear() = 0;

    //! \brief Returns a pointer to the underlying vector object
    //! \returns Pointer to underlying vector object
    virtual void* GetVectorPtr() = 0;

    //! \brief Extends the vector by copying in contents of \c vec
    //! \param vec vector to copy contents from
    //!
    //! Throws throw flame::exceptions::invalid_type if \c vec is of
    //! mismatching type
    virtual void Extend(VectorWrapperBase* vec) = 0;

    //! \brief Return a boost::any object of current type initialsed with value
    //! pointed to by raw pointer.
    //! \param ptr pointer to value to be converted to boost::any
    //! \returns value pointed to by \c ptr wrapped as a boost::any object
    virtual boost::any ConvertToBoostAny(void* ptr) = 0;

    //! \brief Returns a pointer to the Nth element in the internal
    //! array, or NULL if the vector is empty
    //! \param offset
    //! \returns pointer to an specific vector element
    virtual void* GetRawPtr(size_t offset = 0) = 0;

    //! \brief Takes a raw pointer to the internal array element and
    //! returns a pointer to the next next element in the array
    //! or NULL if the end of the array is reached
    //! \param ptr reference data pointer
    //! \returns pointer to the next element that follows \c ptr
    virtual void* StepRawPtr(void* ptr) = 0;

    //! \brief Prints the contents of the vector to the given stream
    virtual void OutputToStream(std::ostream* os, std::string delim) = 0;

    //! \brief Appends to vector a value wrapped in boost::any
    //! \param value value to be appended to the vector
    //! Throws throw flame::exceptions::invalid_type if \c value is of
    //! mismatching type
    virtual void push_back(boost::any value) = 0;

    //! \brief Returns the type info of the underlying datatype
    //! \returns type info
    virtual const std::type_info* GetDataType() const = 0;

    //! \brief Create an empty vector with the same underlying datatype
    //! \returns pointer to the new VectorWrapper object
    virtual VectorWrapperBase* clone_empty() const = 0;

    //! \brief Simulate a virtual copy constructor
    //! \returns pointer to the new VectorWrapper object
    virtual VectorWrapperBase* clone() const = 0;
};

//! \brief Make VectorWrapperBase cloneable within boost::ptr_map
inline VectorWrapperBase* new_clone(const VectorWrapperBase& a) {
  return a.clone();
}

//! \brief Type-specific concrete classes of VectorWrapperBase
//!
//! Used to store and operate on vectors of specific types.
template <typename T>
class VectorWrapper: public VectorWrapperBase {
  public:
    typedef T data_type;  //!< value type
    typedef std::vector<T> vector_type;  //!< vector type

    VectorWrapper() : data_type_(&typeid(T)), v_(0) {}

    //! \brief Copy constructor
    explicit VectorWrapper(const VectorWrapper& v)
      : VectorWrapperBase(v), data_type_(v.data_type_), v_(v.v_) {}

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
      // if offset zero
      if (offset == 0) {
        // if vector empty return NULL else return front
        return (v_.empty()) ? NULL : &(v_.front());
      } else {
        // if offset if larger then vector size then throw exception
        if (offset >= v_.size()) {
          throw flame::exceptions::invalid_argument("invalid offset");
        }
        // return offset of vector
        return &v_[offset];
      }
    }

    boost::any ConvertToBoostAny(void* ptr) {
      return boost::any(*(static_cast<T*>(ptr)));
    }

    void* StepRawPtr(void* ptr) {
      // if ptr NULL or ptr at end of vector
      if (ptr == NULL || ptr == &(v_.back())) {
        return NULL;
      }
      return static_cast<void*>(static_cast<T*>(ptr)+1);
    }

    void OutputToStream(std::ostream* os, std::string delim) {
      typedef typename std::vector<T>::const_iterator const_iterator;
      const_iterator b = v_.begin();
      const_iterator e = v_.end();
      for (const_iterator i = b; i != e; ++i) {
        if (i != b) *os << delim;
        *os << *i;
      }
    }

    const std::type_info* GetDataType() const {
      return data_type_;
    }

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
    const std::type_info *data_type_;  //!< pointer to type_info of current type
    std::vector<T> v_;  //!< underlying container
    //! This class has pointer members so disable assignment operation
    void operator=(const VectorWrapper&);
};


}}  // namespace flame::mem
#endif  // MEM__MEMORY_VECTOR_HPP
