/*!
 * \file flame2/api/message_iterator_wrapper.hpp
 * \author Shawn Chin
 * \date November 2012
 * \copyright Copyright (c) 2012 STFC Rutherford Appleton Laboratory
 * \copyright Copyright (c) 2012 University of Sheffield
 * \copyright GNU Lesser General Public License
 * \brief Wrapper class for flame::mb::MessageIterator
 *
 * This was introduced to enable a cleaner and consistent API. We return a 
 * wrapper instance instead of the shared_ptr returned by board::GetMessages()
 * so users can access iterator methods directly rather than
 * using the -> indirection.
 */
#include <boost/shared_ptr.hpp>
#include "flame2/mem/memory_iterator.hpp"
#ifndef API__MESSAGE_ITERATOR_WRAPPER_HPP_
#define API__MESSAGE_ITERATOR_WRAPPER_HPP_

namespace flame { namespace api {
  
typedef boost::shared_ptr<flame::mb::MessageIterator> SharedMessageIterator;

class MessageIteratorWrapper {
  public:
    explicit MessageIteratorWrapper(SharedMessageIterator iter)
        : parent_(iter) {}
    
    inline bool AtEnd(void) const {
      return parent_->AtEnd();
    }
    
    inline size_t GetCount(void) const {
      return parent_->GetCount();
    }
    
    inline void Rewind(void) {
      parent_->Rewind();
    }
    
    inline bool Next(void) {
      return parent_->Next();
    }
    
    inline void Randomise(void) {
      parent_->Randomise();
    }
    
    template <typename T>
    T GetMessage(void) {
      return parent_->Get<T>();
    }
    
  private:
    SharedMessageIterator parent_;
};

}}  // namespace::api
#endif  // API__MESSAGE_ITERATOR_WRAPPER_HPP_
