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

namespace impl_ { class VectorBase {}; }

template <typename T>
class VectorRW : impl_::VectorBase {
  public:
    VectorRW(std::vector<T> &vec) : vec_(vec) {}
    typedef typename std::vector<T>::iterator iterator;
    // typedef typename std::vector<T>::reverse_iterator reverse_iterator;
    iterator begin() { return vec_.begin(); }
    iterator end() { return vec_.end(); }
    // reverse_iterator rbegin() {return vec_.rbegin(); }
    // reverse_iterator rend() {return vec_.rend(); }
    bool empty() { return vec_.empty(); }
    size_t size() { return vec_.size(); }

  private:
    typename std::vector<T> &vec_;
};

template <typename T>
class VectorRO : impl_::VectorBase {
  public:
    VectorRO(std::vector<T> &vec) : vec_(vec) {}
    typedef typename std::vector<T>::const_iterator iterator;
    // typedef typename std::vector<T>::const_reverse_iterator reverse_iterator;
    iterator begin() { return vec_.begin(); }
    iterator end() { return vec_.end(); }
    // reverse_iterator rbegin() {return vec_.rbegin(); }
    // reverse_iterator rend() {return vec_.rend(); }
    bool empty() { return vec_.empty(); }
    size_t size() { return vec_.size(); }

  private:
    typename std::vector<T> const& vec_;
};
}}  // namespace flame::mem
#endif  // MEM__MEMORY_VECTOR_HPP
