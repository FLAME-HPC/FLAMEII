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

namespace flame { namespace mb2 {


class BoardWriter {
  friend class MessageBoard;
  
  public:
    typedef boost::shared_ptr<BoardWriter> handle;

    size_t GetCount(void);
    bool IsConnected(void);
    
    template <typename T>
    void Post(T& msg) {
#ifndef DISABLE_RUNTIME_TYPE_CHECKING
      if (*(_data->GetDataType()) != typeid(T)) {
        throw flame::exceptions::invalid_type("mismatching type");
      }
#endif
#ifdef DEBUG
      if (!IsConnected()) {
        throw flame::exceptions::logic_error("No longer connected to board");
      }
#endif
      std::vector<T> *v = static_cast<std::vector<T>*>(_data->GetVectorPtr());
      v->push_back(msg);
    }
    
  protected:
    boost::scoped_ptr<VectorWrapperBase> _data;

    // Constructor should only be called by MessageBoard
    template <typename T>
    BoardWriter(void) : _data(new VectorWrapper<T>()), _connected(true) {}

    BoardWriter(BoardWriter* src)
        : _data(src->_data->clone_empty()), _connected(true) {}

    void Disconnect();

  private:
    bool _connected;
}

}}  // namespace flame::mb2
#endif  // MB__BOARD_WRITER_HPP
