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

    size_t GetCount(void) const;
    bool IsConnected(void) const;

    template <class T>
    static BoardWriter* create(void) {
      return new BoardWriter(new flame::mem::VectorWrapper<T>());
    }
    
    template <typename T>
    void Post(const T &msg) {
#ifndef DISABLE_RUNTIME_TYPE_CHECKING
      if (*(_data->GetDataType()) != typeid(T)) {
        throw flame::exceptions::invalid_type("mismatching type");
      }
#endif
#ifdef DEBUG
      if (!IsConnected()) {
        throw flame::exceptions::invalid_operation(
                                 "No longer connected to board");
      }
#endif
      std::vector<T> *v = static_cast<std::vector<T>*>(_data->GetVectorPtr());
      v->push_back(msg);
    }
    
  protected:
    boost::scoped_ptr<flame::mem::VectorWrapperBase> _data;
    
    BoardWriter* clone_empty(void);

    void Disconnect();

  private:
    bool _connected;

    // Since we cannot have templated constructors, all instantiation should
    // be done using the factory function: create<T>()
    explicit BoardWriter(flame::mem::VectorWrapperBase *vec)
        : _data(vec), _connected(true) {}
};

}}  // namespace flame::mb2
#endif  // MB__BOARD_WRITER_HPP
