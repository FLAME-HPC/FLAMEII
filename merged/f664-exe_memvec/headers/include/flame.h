/*!
 * \file include/flame.h
 * \author Shawn Chin
 * \date 2012
 * \copyright Copyright (c) 2012 STFC Rutherford Appleton Laboratory
 * \copyright Copyright (c) 2012 University of Sheffield
 * \copyright GNU Lesser General Public License
 * \brief C API for FLAME2 User API
 */
#ifndef INCLUDE__FLAME_H_
#define INCLUDE__FLAME_H_

#ifdef __cplusplus
extern "C" {
#endif

#define FLAME_AGENT_FUNC_ARG FLAME_unique_symbol_000_001
#define FLAME_AGENT_FUNC(funcName) int funcName(void* FLAME_AGENT_FUNC_ARG)

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

#ifdef __cplusplus
}
#endif
#endif  /* INCLUDE__FLAME_H_ */
