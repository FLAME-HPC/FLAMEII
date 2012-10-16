/*!
 * \file include/flame.h
 * \author Shawn Chin
 * \date 2012
 * \copyright Copyright (c) 2012 STFC Rutherford Appleton Laboratory
 * \copyright Copyright (c) 2012 University of Sheffield
 * \copyright GNU Lesser General Public License
 * \brief C API for FLAME2 User API
 */
#ifndef HEADERS_INCLUDE_FLAME_H_
#define HEADERS_INCLUDE_FLAME_H_
#include <ostream>

#ifdef __cplusplus
extern "C" {
#endif

#define FLAME_AGENT_ALIVE 0
#define FLAME_AGENT_DEAD  1

/* Signature of FLAME_AGENT_FUNC should match definition of TaskFunction
 * in src/exe/task_interface.hpp
 */
#define FLAME_AGENT_FUNC_ARG FLAME_unique_symbol_000_001
#define FLAME_MESSAGE_BOARD_CLIENT FLAME_unique_symbol_000_002
#define FLAME_AGENT_FUNC(funcName) \
      int funcName(void* FLAME_AGENT_FUNC_ARG, void* FLAME_MESSAGE_BOARD_CLIENT)


/*! Function pointer type for agent transition functions */
typedef FLAME_AGENT_FUNC((*AgentFuncPtr));

/* TODO(lsc): These should be generated per-model for each datatype */
double flame_mem_get_double_actual_(void* FLAME_AGENT_FUNC_ARG, const char* k);
#define flame_mem_get_double(k) flame_mem_get_double_actual_(FLAME_AGENT_FUNC_ARG, k)
void flame_mem_set_double_actual_(void* FLAME_AGENT_FUNC_ARG,
                                  const char* k,
                                  double v);
#define flame_mem_set_double(k, v) flame_mem_set_double_actual_(FLAME_AGENT_FUNC_ARG, k, v)
int flame_mem_get_int_actual_(void* FLAME_AGENT_FUNC_ARG, const char* k);
#define flame_mem_get_int(k) flame_mem_get_int_actual_(FLAME_AGENT_FUNC_ARG, k)
void flame_mem_set_int_actual_(void* FLAME_AGENT_FUNC_ARG,
                               const char* k,
                               int v);
#define flame_mem_set_int(k, v) flame_mem_set_int_actual_(FLAME_AGENT_FUNC_ARG, k, v)


/* ------------------ message board stuff ----------------------------- */
#define flame_msg_post(k, v) flame_msg_post_actual(FLAME_MESSAGE_BOARD_CLIENT, k, v)
void flame_msg_post_actual(void* FLAME_MESSAGE_BOARD_CLIENT, const char* k, void* v);


/* assume all messages have one var */
#define FLAME_MESSAGE_VARNAME "__DATA__"


typedef void* flame_msg_iterator;

#define flame_msg_get_iterator(m) flame_msg_get_iterator_actual(FLAME_MESSAGE_BOARD_CLIENT, m)
flame_msg_iterator flame_msg_get_iterator_actual(void* mb, const char* msg);

int flame_msg_iterator_end(flame_msg_iterator iter);

int flame_msg_iterator_next(flame_msg_iterator iter);

void flame_msg_iterator_get_message(flame_msg_iterator iter, void* msg);

void flame_msg_iterator_free(flame_msg_iterator iter);

/* Must call this once for message types to be registerd.
 * see src/contrib/C/mb_api.cpp
 */
void flame_mb_api_hack_initialise(void);

/* Dummy message type (for testing). In practice, it should be generated per-model */
typedef struct {
  double x, y, z;
  int id;
} location_message;

inline std::ostream &operator<<(std::ostream &os, const location_message& ob) {
  os << "{" << ob.x << ', ' << ob.y << ", " << ob.z << ", " << ob.id << "}";
  return os;
}


#ifdef __cplusplus
}
#endif

#endif  // HEADERS_INCLUDE_FLAME_H_
