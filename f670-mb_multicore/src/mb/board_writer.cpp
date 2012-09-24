#include "exceptions/all.hpp"
#include "board_writer.hpp"
namespace flame { namespace mb {

BoardWriter::BoardWriter(const std::string message_name)
  : msg_name_(message_name) {}

void BoardWriter::RegisterVar(std::string var_name, GenericVector* vec) {
  std::pair<MemoryMap::iterator, bool> ret;
  ret = mem_map_.insert(var_name, vec);
  if (!ret.second) {  // if replacement instead of insertion
    throw flame::exceptions::logic_error("variable already registered");
  }
  type_map_[var_name] = vec->GetDataType();
}


}}  // namespace flame::mb
