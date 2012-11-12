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
#include "flame2/mem/vector_wrapper.hpp"
#include "board_writer.hpp"
#include "message_iterator.hpp"

namespace flame { namespace mb2 {

class MessageBoard {
  public:
    typedef boost::shared_ptr<BoardWriter> writer;
    typedef boost::shared_ptr<MessageIterator> iterator;
    
    template <class T>
    static MessageBoard* create(const std::string& msg_name) {
      return new MessageBoard(msg_name,
                              new flame::mem::VectorWrapper<T>(),
                              BoardWriter::create<T>());
    }

    void Sync(void);
    void Clear(void);
    size_t GetCount(void) const;

    writer GetBoardWriter(void);
    iterator GetMessages(void);
    
  private:
    typedef std::vector<writer> WriterVector;

    std::string name_;
    WriterVector writers_;
    // store a reference board writer of the correct type
    // All writers for this board will be cloned from this one, thus allowing
    // the GetBoardWriter call to be typeless.
    boost::scoped_ptr<BoardWriter> writer_template_;
    boost::scoped_ptr<flame::mem::VectorWrapperBase> data_;

    // Since we cannot have templated constructors, all instantiation should
    // be done using the factory function: create<T>()
    MessageBoard(const std::string& msg_name,
                 flame::mem::VectorWrapperBase *vec,
                 BoardWriter *writer)
        : name_(msg_name), writer_template_(writer), data_(vec) {}
        
    void _DeleteWriters(void);
    
};

}}  // namespace flame::mb2
#endif  // MB__MESSAGE_BOARD_HPP_
