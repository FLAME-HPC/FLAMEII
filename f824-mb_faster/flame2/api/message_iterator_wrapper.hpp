/*!
 * \file flame2/api/message_iterator_wrapper.hpp
 * \author Shawn Chin
 * \date November 2012
 * \copyright Copyright (c) 2012 STFC Rutherford Appleton Laboratory
 * \copyright Copyright (c) 2012 University of Sheffield
 * \copyright GNU Lesser General Public License
 * \brief Wrapper class for flame::mb::MessageIterator
 */
#ifndef FLAME2__API__MESSAGE_ITERATOR_WRAPPER_HPP_
#define FLAME2__API__MESSAGE_ITERATOR_WRAPPER_HPP_
#include <boost/shared_ptr.hpp>
#include "flame2/mem/memory_iterator.hpp"
#include "flame2/exceptions/api.hpp"
#include "flame2/exceptions/all.hpp"

//! For non-production code, assert that the message iterator is not null.
//! To be used at by all methods in MessageIteratorWrapper.
#ifndef NDEBUG
  #define ASSERT_PTR_NOT_NULL(ptr) { \
    if (!ptr) { \
      throw flame::exceptions::flame_api_invalid_operation("MessageIterator", \
        "Iterators must be assigned using " \
        "FLAME.GetMessages(\"message_name\") before it can be accessed."); \
    } \
  }
#else
  #define ASSERT_PTR_NOT_NULL(ptr) do {} while (0)
#endif

namespace flame { namespace api {

//! Shared pointer type for actuals MessageIterator
typedef boost::shared_ptr<flame::mb::MessageIterator> SharedMessageIterator;

/*!
 * \brief Wrapper class for flame::mb::MessageIterator
 *
 * This was introduced to enable a cleaner and consistent API. An instance of
 * this class is returned in place of the shared_ptr returned by
 * flame::mb::MessageBoard::GetMessages() so users can access iterator methods
 * directly rather than using the -> indirection.
 *
 * While not technically useful, we define a default constructor so users
 * can declare an iterator instance without assigning it. This reduces the
 * number of gotchas in the API.
 *
 * \code{.cpp}
 *   MessageIterator iter;  // declare obj without assignment (null ptr)
 *   // ...
 *   iter = FLAME.GetMessageIterator("message_name");  // assign later
 * \endcode
 * 
 * When the default constructor is used, a null shared pointer is created.
 * Dereferencing this will cause an assertion error. It is therefore the users'
 * responsibility to ensure that instances should not be used until assigned
 * a value using the GetMessageIterator() API call.
 *
 * Checking the pointer each time a method is called may be expensive in when
 * done repeatedly in a tight loop. To avoid this overhead, the checks are
 * compiled away in the production version of the library (users are expected
 * to use the debug version for development work).
 */
class MessageIteratorWrapper {
  public:
    //! Default constructor with null pointer
    MessageIteratorWrapper(void) {}

    //! Constructor which initialises the pointer to actual message iterator
    explicit MessageIteratorWrapper(SharedMessageIterator iter)
        : parent_(iter) {}

    //! Returns true if end of iteration reached, false otherwise.
    inline bool AtEnd(void) const {
      ASSERT_PTR_NOT_NULL(parent_);
      return parent_->AtEnd();
    }

    //! Returns the number of messages in the iterator
    inline size_t GetCount(void) const {
      ASSERT_PTR_NOT_NULL(parent_);
      return parent_->GetCount();
    }

    //! Resets the iterator to restart iteration from the beginning
    inline void Rewind(void) {
      ASSERT_PTR_NOT_NULL(parent_);
      parent_->Rewind();
    }

    /*!
     * \brief Steps to the next message in the iteration
     * \return true if successful, false otherwise (end of iteration)
     */
    inline bool Next(void) {
      ASSERT_PTR_NOT_NULL(parent_);
      return parent_->Next();
    }

    /*!
     * \brief Randomises the iterator so messages are returned in random order
     *
     * (Not yet implemented)
     *
     * Note that out of order iteration of messages requires the iterator to
     * store an intermediate array of indices. This may lead to a degradation
     * in iteration performance.
     *
     * Randomisation of an in-progress iterator will rewind it.
     */
    inline void Randomise(void) {
      ASSERT_PTR_NOT_NULL(parent_);
      throw flame::exceptions::flame_api_not_implemented(
        "MessageIterator::Randomise", "Not yet implemented, sorry.");
      parent_->Randomise();
    }

    /*!
     * \brief Return the current message in the iteration
     *
     * Messages are return by-value. This allows for 'safer' usage at the
     * expense of the copy overheads.
     *
     * Throws flame::exceptions::flame_api_out_of_range if called on an empty
     * iterator of if end of iteration is reached.
     *
     * Throws flame::exceptions::flame_api_invalid_type when the type used does
     * not match the datatype of the requested message.
     */
    template <typename T>
    T GetMessage(void) {
      ASSERT_PTR_NOT_NULL(parent_);
      try {
        return parent_->Get<T>();
      } catch(const flame::exceptions::invalid_type& E) {
        throw flame::exceptions::flame_api_invalid_type(
          "MessageIterator::GetMessage",
          "Invalid type specified. Check that the type used when calling "
          "'.GetMessage<MESSAGE_TYPE>()' matches the message type.");
      } catch(const flame::exceptions::out_of_range& E) {
        throw flame::exceptions::flame_api_out_of_range(
          "MessageIterator::GetMessage",
          "End of Iteration. GetMessage<MESSAGE_TYPE>() should not be called "
          "once the iteration has completed or if the message board is empty. "
          "You can check using '.AtEnd()'.");
      }
    }

  private:
    SharedMessageIterator parent_;  // shared pointer to the actual iterator
};

}}  // namespace::api
#endif  // FLAME2__API__MESSAGE_ITERATOR_WRAPPER_HPP_
