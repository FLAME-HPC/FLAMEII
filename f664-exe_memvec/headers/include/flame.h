/*!
 * \file include/flame.h
 * \author Shawn Chin
 * \date 2012
 * \copyright Copyright (c) 2012 STFC Rutherford Appleton Laboratory
 * \copyright Copyright (c) 2012 University of Sheffield
 * \copyright GNU Lesser General Public License
 * \brief Header file for FLAME2 User API
 */
#ifndef INCLUDE__FLAME_H_
#define INCLUDE__FLAME_H_

#ifdef __cplusplus
extern "C" {
#endif

/*
#define FLAME_AGENT_FUNC(funcName) int funcName(FLAME_afunc_idx IDX_000_001, FLAME_afunc_mem MEM_000_001)
#define flame_get_mem_dbl(k) flame_get_mem_dbl_actual(IDX_000_001, MEM_000_001, k)
#define flame_set_mem_dbl(k, v) flame_set_mem_dbl_actual(IDX_000_001, MEM_000_001, k, v)
#define flame_get_mem_int(k) flame_get_mem_int_actual(IDX_000_001, MEM_000_001, k)
#define flame_set_mem_int(k, v) flame_set_mem_int_actual(IDX_000_001, MEM_000_001, k, v)
*/

// Note: This is a mock implementation which only handles one agent memory var
#define FLAME_AGENT_FUNC(funcName) int funcName(int* mem)

/*! Function pointer type for agent transition functions */
typedef FLAME_AGENT_FUNC((*AgentFuncPtr));


#ifdef __cplusplus
}
#endif
#endif  /* INCLUDE__FLAME_H_ */
