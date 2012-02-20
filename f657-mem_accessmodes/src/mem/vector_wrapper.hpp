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
class VectorWrapperBase {};

//! Proxy object for memory vector which provides Read-Write access
template <typename T>
class VectorWriter : VectorWrapperBase {
  public:
    VectorWriter(std::vector<T> &vec) : vec_(vec) {}
    typedef typename std::vector<T>::iterator iterator;

    iterator begin() const { return vec_.begin(); }
    iterator end() const { return vec_.end(); }
    bool empty() const { return vec_.empty(); }
    size_t size() const { return vec_.size(); }

  private:
    typename std::vector<T> &vec_;
};

//! Proxy object for memory vector which provides Readonly access
template <typename T>
class VectorReader : VectorWrapperBase {
  public:
    VectorReader(std::vector<T> &vec) : vec_(vec) {}
    typedef typename std::vector<T>::const_iterator iterator;

    iterator begin() const { return vec_.begin(); }
    iterator end() const { return vec_.end(); }
    bool empty() const { return vec_.empty(); }
    size_t size() const { return vec_.size(); }

  private:
    typename std::vector<T> const& vec_;
};
}}  // namespace flame::mem
#endif  // MEM__MEMORY_VECTOR_HPP
