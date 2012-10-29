/*!
 * \file flame2/compat/C/flame_compat_c.cpp
 * \author Shawn Chin
 * \date October 2012
 * \copyright Copyright (c) 2012 STFC Rutherford Appleton Laboratory
 * \copyright Copyright (c) 2012 University of Sheffield
 * \copyright GNU Lesser General Public License
 * \brief C compatibility routines
 */
#include <string>
#include "flame2/mb/client.hpp"
#include "compatibility_manager.hpp"
#include "flame_compat_c.hpp"
namespace flame { namespace compat { namespace c {

// -------------- Declaration of constants -----------

//! Name used as var for message boards registered via CompatibilityManager
const std::string Constants::MESSAGE_VARNAME = "__COMPAT_C_MESSAGE_DATA__";

}}}  // namespace flame::compat::c

// ============== outside flame::compat::c namespace ===================

namespace com  = flame::compat::c;

#ifdef __cplusplus
extern "C" {
#endif

// -------------- Memory Access APIs ------------------
// The following are hardcoded routines to support int/double/float
// To support user defined datatypes (structs), the parser should
//  generate similar routines for each type

int flame_mem_get_int_actual_(void* mem, const char* key) {
  return flame_mem_get<int>(mem, key);
}
void flame_mem_set_int_actual_(void* mem, const char* key, int value) {
  flame_mem_set<int>(mem, key, value);
}

double flame_mem_get_double_actual_(void* mem, const char* key) {
  return flame_mem_get<double>(mem, key);
}
void flame_mem_set_double_actual_(void* mem, const char* key, double value) {
  flame_mem_set<double>(mem, key, value);
}

float flame_mem_get_float_actual_(void* mem, const char* key) {
  return flame_mem_get<float>(mem, key);
}
void flame_mem_set_float_actual_(void* mem, const char* key, float value) {
  flame_mem_set<float>(mem, key, value);
}


// -------------- Message Board APIs ------------------

//! Posts a message using data store in memory location msg_ptr
void flame_msg_post_actual_(void* mb_client,
                            std::string msg_name,
                            void* msg_ptr) {
  flame::mb::Client* client = static_cast<flame::mb::Client*>(mb_client);
  com::CompatibilityManager& compat = com::CompatibilityManager::GetInstance();
  com::MessageBoardWrapper *board =  compat.GetBoardWrapper(msg_name);
  board->PostMessage(client, msg_ptr);
}

//! Retrieves a message iterator for a specific message
flame_msg_iterator flame_msg_get_iterator_actual_(void* mb_client,
                                                  std::string msg_name) {
  flame::mb::Client* client = static_cast<flame::mb::Client*>(mb_client);
  com::CompatibilityManager& compat = com::CompatibilityManager::GetInstance();
  com::MessageBoardWrapper *board =  compat.GetBoardWrapper(msg_name);
  return static_cast<flame_msg_iterator>(board->GetMessages(client));
}

//! Progress the iterator to the next message. Returns false if end reached.
int flame_msg_iterator_next(flame_msg_iterator iter) {
  return static_cast<com::MessageIteratorWrapper*>(iter)->Next();
}

//! Returns true if the iteration has ended
int flame_msg_iterator_end(flame_msg_iterator iter) {
  return static_cast<com::MessageIteratorWrapper*>(iter)->AtEnd();
}

//! Deletes the iterator object
void flame_msg_iterator_free(flame_msg_iterator iter) {
  delete static_cast<com::MessageIteratorWrapper*>(iter);
}

//! Writes the current message to memory location, m_ptr
void flame_msg_iterator_get_message(flame_msg_iterator iter, void* m_ptr) {
  static_cast<com::MessageIteratorWrapper*>(iter)->GetMessage(m_ptr);
}
#ifdef __cplusplus
}
#endif


