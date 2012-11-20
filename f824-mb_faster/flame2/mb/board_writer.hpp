/*!
 * \file flame2/mb/board_writer.hpp
 * \author Shawn Chin
 * \date November 2012
 * \copyright Copyright (c) 2012 STFC Rutherford Appleton Laboratory
 * \copyright Copyright (c) 2012 University of Sheffield
 * \copyright GNU Lesser General Public License
 * \brief Declaration of BoardWriter class
 */
#ifndef MB__BOARD_WRITER_HPP
#define MB__BOARD_WRITER_HPP
#include <vector>
#include <boost/shared_ptr.hpp>
#include <boost/scoped_ptr.hpp>
#include "flame2/exceptions/all.hpp"
#include "flame2/mem/vector_wrapper.hpp"

namespace flame { namespace mb {

/*!
 * \brief Object acting as a buffer for posts to a message board
 *
 * Access to this object is not mutex protected as each worker thread is
 * expected to request its own instance.
 */
class BoardWriter {
  friend class MessageBoard;  //! Give MessageBoard access to protected methods
  
  public:
    //! Returns the number of posted messages
    size_t GetCount(void) const;

    /*!
     * \brief Returns true if still connected to parent board
     *
     * Once disconnected, messages can no longer be posted.
     */
    inline bool IsConnected(void) const { return connected_; }

    /*!
     * \brief Posts a message
     *
     * Posted message will be buffered in the writer and only copied to the
     * message board during a message board sync.
     *
     * Throws flame::exceptions::invalid_type if the type specified does not
     * match the message type.
     *
     * Throws flame::exceptions::invalid_operation if the the writer has been
     * disconnected
     */
    template <typename T>
    void Post(const T &msg) {
#ifndef DISABLE_RUNTIME_TYPE_CHECKING
      if (*(data_->GetDataType()) != typeid(T)) {
        throw flame::exceptions::invalid_type("mismatching type");
      }
#endif
#ifdef DEBUG
      if (!IsConnected()) {
        throw flame::exceptions::invalid_operation(
                                 "No longer connected to board");
      }
#endif
      std::vector<T> *v = static_cast<std::vector<T>*>(data_->GetVectorPtr());
      v->push_back(msg);
    }
    
  protected:
    //! datatype for smart pointer to VectorWrapper
    boost::scoped_ptr<flame::mem::VectorWrapperBase> data_;
    
    //! Factory method to create a BoardWriter for a specific message type
    template <class T>
    static BoardWriter* create(void) {
      return new BoardWriter(new flame::mem::VectorWrapper<T>());
    }

    //! Return a clone holding the same VectorWrapper type but with no content
    BoardWriter* clone_empty(void);

    //! Disconnect the writer so messages can no longer be posted
    void Disconnect();

  private:
    bool connected_;  //! Flag to determine if the writer is still connected

    //! Private constructor with takes a pointer to a VectorWrapper object
    explicit BoardWriter(flame::mem::VectorWrapperBase *vec)
        : data_(vec), connected_(true) {}
};

}}  // namespace flame::mb
#endif  // MB__BOARD_WRITER_HPP
