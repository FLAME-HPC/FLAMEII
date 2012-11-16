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
 *
 * Note that without a default constuctor, users will not be able to use:
 *   MessageIterator iter;  // null iterator
 */
#ifndef FLAME2__API__MESSAGE_ITERATOR_WRAPPER_HPP_
#define FLAME2__API__MESSAGE_ITERATOR_WRAPPER_HPP_
#include <boost/shared_ptr.hpp>
#include "flame2/mem/memory_iterator.hpp"
#include "flame2/exceptions/api.hpp"
#include "flame2/exceptions/all.hpp"

#ifndef NDEBUG
  #define ASSERT_PTR_NOT_NULL(ptr) { \
    if (!ptr) { \
      throw flame::exceptions::flame_api_invalid_operation("MessageIterator", \
        "Iterators must be assigned using " \
        "FLAME.GetMessages(\"message_name\") before it can be accessed."); \
    } \
  }
#else
  #define ASSERT_PTR_NOT_NULL(ptr) do {} while(0)
#endif

namespace flame { namespace api {
  
typedef boost::shared_ptr<flame::mb::MessageIterator> SharedMessageIterator;

class MessageIteratorWrapper {
  public:
    MessageIteratorWrapper(void) {}  
    
    explicit MessageIteratorWrapper(SharedMessageIterator iter)
        : parent_(iter) {}
    
    inline bool AtEnd(void) const {
      ASSERT_PTR_NOT_NULL(parent_);
      return parent_->AtEnd();
    }
    
    inline size_t GetCount(void) const {
      ASSERT_PTR_NOT_NULL(parent_);
      return parent_->GetCount();
    }
    
    inline void Rewind(void) {
      ASSERT_PTR_NOT_NULL(parent_);
      parent_->Rewind();
    }
    
    inline bool Next(void) {
      ASSERT_PTR_NOT_NULL(parent_);
      try {
        return parent_->Next();
      } catch(const flame::exceptions::out_of_range& E) {
        throw flame::exceptions::flame_api_out_of_range(
          "MessageIterator::Next",
          "End of Iteration. Next() should not be called once the "
          "iteration has completed or if the message board is empty. "
          "You can check using '.AtEnd()'."
        );
      }
    }
    
    inline void Randomise(void) {
      ASSERT_PTR_NOT_NULL(parent_);
      throw flame::exceptions::flame_api_not_implemented(
        "MessageIterator::Randomise", "Not yet implemented, sorry."
      );
      parent_->Randomise();
    }
    
    template <typename T>
    T GetMessage(void) {
      ASSERT_PTR_NOT_NULL(parent_);
      try {
        return parent_->Get<T>();
      } catch(const flame::exceptions::invalid_type& E) {
        throw flame::exceptions::flame_api_invalid_type(
          "MessageIterator::GetMessage",
          "Invalid type specified. Check that the type used when calling "
          "'.GetMessage<MESSAGE_TYPE>()' matches the message type."
        );
      } catch(const flame::exceptions::out_of_range& E) {
        throw flame::exceptions::flame_api_out_of_range(
          "MessageIterator::GetMessage",
          "End of Iteration. GetMessage<MESSAGE_TYPE>() should not be called "
          "once the iteration has completed or if the message board is empty. "
          "You can check using '.AtEnd()'."
        );
      }
    }
    
  private:
    SharedMessageIterator parent_;
};

}}  // namespace::api
#endif  // FLAME2__API__MESSAGE_ITERATOR_WRAPPER_HPP_
