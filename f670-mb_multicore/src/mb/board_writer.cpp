#include <cassert>
#include "boost/bind.hpp"
#include "boost/foreach.hpp"
#include "exceptions/all.hpp"
#include "message.hpp"
#include "board_writer.hpp"
namespace flame { namespace mb {

BoardWriter::BoardWriter(const std::string message_name)
  : msg_name_(message_name) {}


/*! \brief Registers a message var and assign it to the given vector
 *
 * vec should be a pointer to an empty VectorWrapper. It should be assigned
 * from the output of .clone_empty() from the main message vector
 * so it is of the correct type.
 */
void BoardWriter::RegisterVar(std::string var_name, GenericVector* vec) {
  std::pair<MemoryMap::iterator, bool> ret;
  ret = mem_map_.insert(var_name, vec);
  if (!ret.second) {  // if replacement instead of insertion
    throw flame::exceptions::logic_error("variable already registered");
  }

  // Cache the datatype for use as TypeValidator
  RegisterType(var_name, vec->GetDataType());
}

MessageHandle BoardWriter::GetMessage(void) {
  MessageHandle msg = MessageHandle(new Message(this));
  msg->AssignPostCallback(boost::bind(&BoardWriter::PostCallback, this, _1));
  return msg;
}

void BoardWriter::PostCallback(Message* msg) {

  // First, check that all values are value so we don't end up within
  // inconsistent data vectors on error
#ifndef DISABLE_RUNTIME_TYPE_CHECKING
  BOOST_FOREACH(const MemoryMap::value_type &p, mem_map_) {
    if (msg->data_.find(p.first) == msg->data_.end()) {
      throw flame::exceptions::logic_error("not all values defined");
    }
    // type should already be checked on Message.Set(). It will also be
    // checked in mem_map_[].push_back(...);
  }
#endif

  // Both mem_map_ and msg->data_ should have the set of keys so we should
  // be able to iterate through both simultaneously and get the same keys
  MemoryMap::iterator m_iter = mem_map_.begin();
  Message::DataMap::iterator d_iter = msg->data_.begin();

  for (;m_iter != mem_map_.end(); ++m_iter, ++d_iter) {
    assert(m_iter->first == d_iter->first);  // ensure that keys are the same
    m_iter->second->push_back(d_iter->second);
  }

}

}}  // namespace flame::mb
