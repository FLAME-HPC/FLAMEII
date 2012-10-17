/*!
 * \file src/compat/C/flame_compat_c.cpp
 * \author Shawn Chin
 * \date October 2012
 * \copyright Copyright (c) 2012 STFC Rutherford Appleton Laboratory
 * \copyright Copyright (c) 2012 University of Sheffield
 * \copyright GNU Lesser General Public License
 * \brief C compatibility routines
 */
#include <string>
#include "mb/client.hpp"
#include "compatibility_manager.hpp"
#include "flame_compat_c.hpp"
namespace flame { namespace compat { namespace c {

// -------------- Declaration of constants -----------
const std::string Constants::MESSAGE_VARNAME = "__COMPAT_C_MESSAGE_DATA__";

}}}  // namespace flame::compat::c

// ============== outside flame::compat::c namespace ===================
namespace com  = flame::compat::c;

#ifdef __cplusplus
extern "C" {
#endif

// -------------- Message Board APIs ------------------

void flame_msg_post_actual(void* mb_client,
                           std::string msg_name,
                           void* msg_ptr) {
  flame::mb::Client* client = static_cast<flame::mb::Client*>(mb_client);
  com::CompatibilityManager& compat = com::CompatibilityManager::GetInstance();
  com::MessageBoardWrapper *board =  compat.GetBoardWrapper(msg_name);
  board->PostMessage(client, msg_ptr);
}

flame_msg_iterator flame_msg_get_iterator_actual(void* mb_client,
                                                 std::string msg_name) {
  flame::mb::Client* client = static_cast<flame::mb::Client*>(mb_client);
  com::CompatibilityManager& compat = com::CompatibilityManager::GetInstance();
  com::MessageBoardWrapper *board =  compat.GetBoardWrapper(msg_name);
  return static_cast<flame_msg_iterator>(board->GetMessages(client));
}

int flame_msg_iterator_next(flame_msg_iterator iter) {
  return static_cast<com::MessageIteratorWrapper*>(iter)->Next();
}

int flame_msg_iterator_end(flame_msg_iterator iter) {
  return static_cast<com::MessageIteratorWrapper*>(iter)->AtEnd();
}

void flame_msg_iterator_free(flame_msg_iterator iter) {
  delete static_cast<com::MessageIteratorWrapper*>(iter);
}

void flame_msg_iterator_get_message(flame_msg_iterator iter, void* m_ptr) {
  static_cast<com::MessageIteratorWrapper*>(iter)->GetMessage(m_ptr);
}
#ifdef __cplusplus
}
#endif


