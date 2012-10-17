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
#include "mem/memory_iterator.hpp"

namespace flame { namespace compat { namespace c {

class Constants {
  public:
    static const std::string MESSAGE_VARNAME;
};

}}}  // namespace flame::compat::c

// ============== outside flame::compat::c namespace ===================

template <typename T>
inline int flame_mem_get(void* mem, std::string var_name) {
  return static_cast<flame::mem::MemoryIterator*>(mem)->Get<T>(var_name);
}

template <typename T>
inline void flame_mem_set(void* mem, std::string var_name, T value) {
  static_cast<flame::mem::MemoryIterator*>(mem)->Set<T>(var_name, value);
}


#ifdef __cplusplus
extern "C" {
#endif

#define FLAME_MEMORY_ITERATOR FLAME_unique_symbol_000_001__
#define FLAME_MESSAGE_BOARD_CLIENT FLAME_unique_symbol_000_002__

#define FLAME_AGENT_FUNC(funcName) \
      int funcName(void* FLAME_MEMORY_ITERATOR, void* FLAME_MESSAGE_BOARD_CLIENT)

/*! Function pointer type for agent transition functions */
typedef FLAME_AGENT_FUNC((*AgentFuncPtr));

// -------------- Memory Access APIs ------------------

#define flame_mem_get_int(k)   flame_mem_get_int_actual_(FLAME_MEMORY_ITERATOR, k)
#define flame_mem_set_int(k,v) flame_mem_set_int_actual_(FLAME_MEMORY_ITERATOR,k,v)
int flame_mem_get_int_actual_(void* mem, const char* key);
void flame_mem_set_int_actual_(void* mem, const char* key, int value);
#define flame_mem_get_double(k)   flame_mem_get_double_actual_(FLAME_MEMORY_ITERATOR, k)
#define flame_mem_set_double(k,v) flame_mem_set_double_actual_(FLAME_MEMORY_ITERATOR,k,v)
double flame_mem_get_double_actual_(void* mem, const char* key);
void flame_mem_set_double_actual_(void* mem, const char* key, double value);
#define flame_mem_get_float(k)   flame_mem_get_float_actual_(FLAME_MEMORY_ITERATOR, k)
#define flame_mem_set_float(k,v) flame_mem_set_float_actual_(FLAME_MEMORY_ITERATOR,k,v)
float flame_mem_get_float_actual_(void* mem, const char* key);
void flame_mem_set_float_actual_(void* mem, const char* key, float value);

// -------------- Message Board APIs ------------------


typedef void* flame_msg_iterator;

#define flame_msg_post(msg_name, msg_ptr) \
        flame_msg_post_actual_(FLAME_MESSAGE_BOARD_CLIENT, msg_name, msg_ptr);
void flame_msg_post_actual_(void* mb_client,
                            std::string msg_name,
                            void* msg_ptr);


#define flame_msg_get_iterator(msg) \
        flame_msg_get_iterator_actual_(FLAME_MESSAGE_BOARD_CLIENT, msg);
flame_msg_iterator flame_msg_get_iterator_actual_(void* mb_client,
                                                  std::string msg_name);

int flame_msg_iterator_next(flame_msg_iterator iter);
int flame_msg_iterator_end(flame_msg_iterator iter);
void flame_msg_iterator_free(flame_msg_iterator iter);
void flame_msg_iterator_get_message(flame_msg_iterator iter, void* m_ptr);

#ifdef __cplusplus
}
#endif


#endif  // COMPAT__C__FLAME_COMPAT_C_HPP_
