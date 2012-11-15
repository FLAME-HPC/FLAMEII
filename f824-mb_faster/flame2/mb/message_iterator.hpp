/*!
 * \file flame2/mb/message_iterator.hpp
 * \author Shawn Chin
 * \date November 2012
 * \copyright Copyright (c) 2012 STFC Rutherford Appleton Laboratory
 * \copyright Copyright (c) 2012 University of Sheffield
 * \copyright GNU Lesser General Public License
 * \brief Declaration of MessageIterator
 */
#ifndef MB__MESSAGE_ITERATOR_HPP_
#define MB__MESSAGE_ITERATOR_HPP_
#include <boost/scoped_ptr.hpp>
#include <boost/shared_ptr.hpp>
#include "flame2/mem/vector_wrapper.hpp"
#include "flame2/exceptions/all.hpp"
#include "message_iterator_backend.hpp"
#include "message_iterator_backend_raw.hpp"

namespace flame { namespace mb {

class MessageIterator {
  public:
    typedef boost::shared_ptr<MessageIterator> handle;

    // default to a raw backend
    static MessageIterator* create(flame::mem::VectorWrapperBase* vw_ptr) {
      return new MessageIterator(
        MessageIteratorBackend::create<MessageIteratorBackendRaw>(vw_ptr)
      );
    }

    //  Factory function for specific backends
    //template <typename BackendType>
    //static MessageIterator* create(VectorWrapperBase* vw_ptr) {
    //  return new MessageIterator(
    //      MessageIteratorBackend::create<BackendType>(vw_ptr)
    //  );
    //}
    
    inline bool AtEnd(void) const {
      return backend_->AtEnd();
    }
    
    inline size_t GetCount(void) const {
      return backend_->GetCount();
    }
    
    inline void Rewind(void) {
      backend_->Rewind();
    }
    
    inline bool Next(void) {
      return backend_->Next();
    }
    
    inline void Randomise(void) {
      _RequireMutableBackend(); 
      backend_->Randomise();
    }
    
    template <typename T>
    T Get(void) {
#ifndef DISABLE_RUNTIME_TYPE_CHECKING
      if (*(backend_->GetDataType()) != typeid(T)) {
        throw flame::exceptions::invalid_type("mismatching type");
      }
#endif
      if (AtEnd()) {
        throw flame::exceptions::out_of_range("End of iteration");
      }
      return T(*static_cast<T*>(backend_->Get()));
    }
    
  private:
    boost::scoped_ptr<MessageIteratorBackend> backend_;

    MessageIterator(MessageIteratorBackend *b) : backend_(b) {}
    
    inline void _RequireMutableBackend(void) {
      if (backend_->IsMutable()) return;
      boost::scoped_ptr<MessageIteratorBackend> b(backend_->GetMutableVersion());
      backend_.swap(b);
    }
};

}}  // namespace flame::mb
#endif  // MB__MESSAGE_ITERATOR_HPP_
