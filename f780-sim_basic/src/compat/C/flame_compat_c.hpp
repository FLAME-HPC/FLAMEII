/*!
 * \file src/compat/C/flame_compat_c.hpp
 * \author Shawn Chin
 * \date October 2012
 * \copyright Copyright (c) 2012 STFC Rutherford Appleton Laboratory
 * \copyright Copyright (c) 2012 University of Sheffield
 * \copyright GNU Lesser General Public License
 * \brief C compatibility routines
 */
#ifndef COMPAT__C__FLAME_COMPAT_C_HPP_
#define COMPAT__C__FLAME_COMPAT_C_HPP_
#include <string>

namespace flame { namespace compat { namespace c {

class Constants {
  public:
    static const std::string MESSAGE_VARNAME;
};

}}}  // namespace flame::compat::c

// ============== outside flame::compat::c namespace ===================

#ifdef __cplusplus
extern "C" {
#endif

// -------------- Message Board APIs ------------------
typedef void* flame_msg_iterator;

#define flame_msg_post(msg_name, msg_ptr) \
        flame_msg_post_actual(FLAME_MESSAGE_BOARD_CLIENT, msg_name, msg_ptr);
void flame_msg_post_actual(void* mb_client,
                           std::string msg_name,
                           void* msg_ptr);


#define flame_msg_get_iterator(msg) \
        flame_msg_get_iterator_actual(FLAME_MESSAGE_BOARD_CLIENT, msg);
flame_msg_iterator flame_msg_get_iterator_actual(void* mb_client,
                                                 std::string msg_name);

int flame_msg_iterator_next(flame_msg_iterator iter);
int flame_msg_iterator_end(flame_msg_iterator iter);
void flame_msg_iterator_free(flame_msg_iterator iter);
void flame_msg_iterator_get_message(flame_msg_iterator iter, void* m_ptr);

#ifdef __cplusplus
}
#endif


#endif  // COMPAT__C__FLAME_COMPAT_C_HPP_
