/*!
 * \file flame2/mb2/message_iterator_backend_raw.cpp
 * \author Shawn Chin
 * \date November 2012
 * \copyright Copyright (c) 2012 STFC Rutherford Appleton Laboratory
 * \copyright Copyright (c) 2012 University of Sheffield
 * \copyright GNU Lesser General Public License
 * \brief Implementation of an Iterator Backend that references the original
 * message data and steps through using pointer arithmetic.
 */
#include "flame2/exceptions/all.hpp"
#include "flame2/mem/vector_wrapper.hpp"
#include "flame2/config.hpp"
#include "message_iterator_backend_raw.hpp"

namespace flame { namespace mb2 {
  MessageIteratorBackendRaw::MessageIteratorBackendRaw(
      flame::mem::VectorWrapperBase* vw_ptr)
          : _v(vw_ptr),
            _count(vw_ptr->size()),
            _pos(0),
            _current(vw_ptr->GetRawPtr()) {}

  MessageIteratorBackendRaw::~MessageIteratorBackendRaw(void) {}

  bool MessageIteratorBackendRaw::AtEnd(void) const {
    return (_pos == _count);
  }

  size_t MessageIteratorBackendRaw::GetCount(void) const {
    return _count;
  }

  void MessageIteratorBackendRaw::Rewind(void) {
    _pos = 0;
    _current = _v->GetRawPtr();
  }

  bool MessageIteratorBackendRaw::Next(void) {
    if (_pos == _count) {
      throw flame::exceptions::out_of_range("End of iteration");
    }
    _pos++;
    _current = _v->StepRawPtr(_current);
    return (_pos != _count);  // return false if end of iter, true otherwise
  }

  bool MessageIteratorBackendRaw::Randomise(void) {
    throw flame::exceptions::logic_error("Raw backend cannot be randomised");
  }

  void* MessageIteratorBackendRaw::Get(void) {
    return _current;
  }
  
  bool MessageIteratorBackendRaw::IsMutable(void) const {
    return false;
  }
  
  MessageIteratorBackend*
  MessageIteratorBackendRaw::GetMutableVersion(void) const {
    throw flame::exceptions::not_implemented("not yet implemented");
  }
  
}}  // namespace flame::mb2
