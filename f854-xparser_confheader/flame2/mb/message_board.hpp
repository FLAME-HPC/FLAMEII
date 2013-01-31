/*!
 * \file flame2/mb/message_board.hpp
 * \author Shawn Chin
 * \date November 2012
 * \copyright Copyright (c) 2012 STFC Rutherford Appleton Laboratory
 * \copyright Copyright (c) 2012 University of Sheffield
 * \copyright GNU Lesser General Public License
 * \brief Declaration of MessageBoard class
 */
#ifndef MB__MESSAGE_BOARD_HPP_
#define MB__MESSAGE_BOARD_HPP_
#include <string>
#include <vector>
#include <boost/scoped_ptr.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/thread/mutex.hpp>
#include "flame2/mem/vector_wrapper.hpp"
#include "board_writer.hpp"
#include "message_iterator.hpp"

namespace flame { namespace mb {

/*!
 * \brief Representation of a message board
 *
 * Each instance represents a message board storing messages of a particular
 * type. The message board instance itself is type-agnostic and relies on
 * the underlying VectorWrapper to handle type-specific operations. This allows
 * us to treat all boards uniformly regardless of what message type it holds.
 */
class MessageBoard {
  public:
    //! shared pointer type to reference a board writer
    typedef boost::shared_ptr<BoardWriter> writer;
    //! shared pointer type to reference a message iterator
    typedef boost::shared_ptr<MessageIterator> iterator;

    /*!
     * \brief Factory method to create a board storing messages of a
     * specific type
     *
     * We need to know the datatype of a message during instantiation so we can
     * assign an appropriate VectorWrapper to the board. We do not want to
     * whole MessageBoard class to be templated, only the constrction process,
     * so we we rely on a templated factory method.
     *
     * This method instantiates a VectorWrapper of the correct type and passes
     * on a base pointer to the private constructor to produce a MessageBoard
     * instance.
     */
    template <class T>
    static MessageBoard* create(const std::string& msg_name) {
      return new MessageBoard(msg_name,
                              new flame::mem::VectorWrapper<T>(),
                              BoardWriter::create<T>());
    }

    /*!
     * \brief Synchronises the content of the message board
     *
     * To support distributed writes and efficient concurrent writes, posted
     * messages may be buffered internally. A call to Sync() will collate the
     * messages, and in the case of distributed message boards, make all
     * required messages accessible from remote nodes. 
     *
     * \note All BoardWriter instances issued before the sync will be
     * disconnected and can no longer be used to post messages. New writes must
     * be requested for subsequent writes.
     */
    void Sync(void);

    /*!
     * \brief Empties the message board
     *
     * \note All BoardWriter instances issued before the sync will be
     * disconnected and can no longer be used to post messages. New writes must
     * be requested for subsequent writes.
     */ 
    void Clear(void);

    /*!
     * \brief Returns the number of messages within the board
     *
     * Note that messages posted after the last Sync() may not be taken into
     * account.
     */
    size_t GetCount(void) const;

    /*!
     * \brief Returns an BoardWriter instance
     * 
     * Writes to the board are buffered and not immediately accessible. This
     * allows writes to be performed concurrently and in a distributed manner
     * without expensive locking and repeated communication.
     *
     * Messages posted to the BoardWriter is not guaranteed to be accessible
     * from the board until a Sync() is called.
     */
    writer GetBoardWriter(void);

    /*!
     * \brief Returns a message iterator
     *
     * The returned iterator can be used to iterate through messages that are
     * in the board at the point where the iterator was created. Any messages
     * added after the iterator was created will not be accessible.
     *
     * A call to Clear() will invalidate all iterators and it is the users
     * responsiblity to ensure that those iterators are no longer used.
     * Accessing an invalidated iterator will result is undefined behaviour
     * and may result in memory violation errors.
     */
    iterator GetMessages(void);

  private:
    //! datatype for storing collection of active board writers
    typedef std::vector<writer> WriterVector;

    std::string name_;  //! message name
    WriterVector writers_;  //! collection of active board writers
    // store a reference board writer of the correct type
    // All writers for this board will be cloned from this one, thus allowing
    // the GetBoardWriter call to be typeless.

    /*!
     * \brief Pointer to empty BoardWriter with a correct vector type attached
     *
     * This is used as a template for cloning new writers without having to
     * specify the type.
     */
    boost::scoped_ptr<BoardWriter> writer_template_;

    //! Pointer to VectorWrapper instance to store messages
    boost::scoped_ptr<flame::mem::VectorWrapperBase> data_;

    boost::mutex mutex_;  //! mutex lock for orchestrating concurrent access

    //! Internal constructor used by factory method to instantiate board
    MessageBoard(const std::string& msg_name,
                 flame::mem::VectorWrapperBase *vec,
                 BoardWriter *board_writer)
        : name_(msg_name), writer_template_(board_writer), data_(vec) {}

    //! Internal routine used to disconnect and delete collection of writers
    void _DeleteWriters(void);
};

}}  // namespace flame::mb
#endif  // MB__MESSAGE_BOARD_HPP_
