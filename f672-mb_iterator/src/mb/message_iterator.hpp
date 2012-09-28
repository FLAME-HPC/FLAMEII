/*!
 * \file src/mb/message_iterator.hpp
 * \author Shawn Chin
 * \date September 2012
 * \copyright Copyright (c) 2012 STFC Rutherford Appleton Laboratory
 * \copyright Copyright (c) 2012 University of Sheffield
 * \copyright GNU Lesser General Public License
 * \brief Declaration of MessageIterator
 */
#ifndef MB__MESSAGE_ITERATOR_HPP_
#define MB__MESSAGE_ITERATOR_HPP_
#include <string>
#include "type_validator.hpp"
#include "mb_common.hpp"
#include "message_iterator_backend.hpp"

namespace flame { namespace mb {

class MessageIterator {
  public:
    explicit MessageIterator(MessageIteratorBackend::Handle backend);
    bool AtEnd(void) const;
    void Next(void);
    void Rewind(void);
    void Randomise(void);
    MessageHandle GetMessage(void) const;

    /*
    template <typename T>
    T Get(const std::string& var_name) {
      return current_->Get<T>(var_name);
    }
    */
  private:
    MessageHandle current_;
    MessageIteratorBackend::Handle backend_;
};

}}  // namespace flame::mb
#endif  // MB__MESSAGE_ITERATOR_HPP_
