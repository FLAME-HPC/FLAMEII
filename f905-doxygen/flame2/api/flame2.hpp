/*!
 * \file flame2/api/flame2.hpp
 * \author Shawn Chin
 * \date November 2012
 * \copyright Copyright (c) 2012 STFC Rutherford Appleton Laboratory
 * \copyright Copyright (c) 2012 University of Sheffield
 * \copyright GNU Lesser General Public License
 * \brief C++ API for FLAME2 Models
 */
#ifndef FLAME2__API__FLAME2_HPP_
#define FLAME2__API__FLAME2_HPP_
#include "flame2/api/agent_api.hpp"

//! Return code for agent function to remain alive 
#define FLAME_AGENT_ALIVE 0
//! Return code for agent function to trigger death of agent
#define FLAME_AGENT_DEAD  1

//! Message Iterator type to be used in user code
typedef flame::api::MessageIteratorWrapper MessageIterator;
//! Expose datatype for API proxy object
using flame::api::AgentAPI;
//! input argument type expected of all agent transition function
using flame::api::FLAME_AgentFunctionParamType;
//! return type expected of all agent transition function
using flame::api::FLAME_AgentFunctionReturnType;

/*! \brief Macro for defining agent transition functions with the
 * correct input argument and return type
 */
#define FLAME_AGENT_FUNCTION(funcName) \
          FLAME_AgentFunctionReturnType \
          funcName(FLAME_AgentFunctionParamType FLAME)


#endif  // FLAME2__API__FLAME2_HPP_
