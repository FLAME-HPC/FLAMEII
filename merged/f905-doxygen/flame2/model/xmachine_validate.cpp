/*!
 * \file flame2/model/xmachine_validate.cpp
 * \author Simon Coakley
 * \date 2013
 * \copyright Copyright (c) 2013 STFC Rutherford Appleton Laboratory
 * \copyright Copyright (c) 2013 University of Sheffield
 * \copyright GNU Lesser General Public License
 * \brief XMachineValidate: holds agent validator
 */
#include <string>
#include <utility>
#include <vector>
#include <set>
#include "flame2/config.hpp"
#include "xmachine_validate.hpp"
#include "validate_name.hpp"
#include "xvariables_validate.hpp"

namespace flame { namespace model {

XMachineValidate::XMachineValidate(
    const boost::ptr_vector<XDataType>& datatypes,
    boost::ptr_vector<XMessage>& messages,
    boost::ptr_vector<XTimeUnit>& timeUnits)
  : datatypes_(datatypes), messages_(messages), timeUnits_(timeUnits) {}

int XMachineValidate::validateAgent(XMachine * agent) {
  int rc, errors = 0;
  boost::ptr_vector<XFunction>::iterator fit;
  std::string name = agent->getName();

  /* Check name is valid */
  if (!ValidateName::name_is_allowed(name)) {
    printErr("Error: Agent name is not valid: %s\n", name.c_str());
    ++errors;
  }

  /* Validate variables */
  XVariablesValidate xvariablesValidate(datatypes_);
  errors += xvariablesValidate.validateVariables(agent->getVariables(), true);

  /* Validate agent functions */
  for (fit = agent->getFunctions()->begin();
      fit != agent->getFunctions()->end(); ++fit) {
    rc = validateAgentFunction(&(*fit), agent);
    if (rc != 0) {
      printErr("\tfrom function: %s\n", (*fit).getName().c_str());
      errors += rc;
    }
  }

  // Validate agent state graph
  errors += validateAgentStateGraph(agent);

  if (errors > 0)  printErr("\tfrom agent: %s\n", name.c_str());

  return errors;
}

bool XMachineValidate::variableExists(std::string name,
    boost::ptr_vector<XVariable> * variables) {
  boost::ptr_vector<XVariable>::iterator vit;

  for (vit = variables->begin(); vit != variables->end(); ++vit)
    if (name == (*vit).getName()) return true;
  return false;
}

int XMachineValidate::processMemoryAccessVariable(std::string name,
    boost::ptr_vector<XVariable> * variables,
    std::set<std::string> * usedVariables) {
  int errors = 0;

  // If variable exists
  if (variableExists(name, variables)) {
    // Check if variable already used
    std::set<std::string>::iterator it = usedVariables->find(name);
    // If not already used
    if (it == usedVariables->end()) {
      // Add variable name to used list
      usedVariables->insert(name);
    } else {
      // If variable already used
      printErr("Error: Memory access variable name is a duplicate: %s\n",
          name.c_str());
      ++errors;
    }

  } else {
    // If variable does not exist
    printErr("Error: Memory access variable name is not valid: %s\n",
        name.c_str());
    ++errors;
  }

  return errors;
}

int XMachineValidate::processAgentFunction(XFunction * function,
    boost::ptr_vector<XVariable> * variables) {
  std::vector<std::string>::iterator variable;
  boost::ptr_vector<XVariable>::iterator variable2;
  std::set<std::string> usedVariables;
  int errors = 0;

  // If memory access information was not given set all memory
  // variable access as being read write.
  if (!function->getMemoryAccessInfoAvailable()) {
    // output a warning
    printErr("Warning: Agent function does not have memory access info: %s\n"
      "         Defaulting memory access to read write for all agent memory\n",
      function->getName().c_str());
    function->setMemoryAccessInfoAvailable(true);
    for (variable2 = variables->begin();
        variable2 != variables->end(); ++variable2)
      function->addReadWriteVariable((*variable2).getName());
    // Else check memory access variables are valid
  } else {
    // Check variable names are valid variables in memory
    for (variable = function->getReadOnlyVariables()->begin();
        variable != function->getReadOnlyVariables()->end(); ++variable)
      errors += processMemoryAccessVariable(
          (*variable), variables, &usedVariables);
    for (variable = function->getReadWriteVariables()->begin();
        variable != function->getReadWriteVariables()->end(); ++variable)
      errors += processMemoryAccessVariable(
          (*variable), variables, &usedVariables);
  }

  return errors;
}

int XMachineValidate::validateAgentFunctionIOput(XFunction * xfunction,
    XMachine * agent) {
  boost::ptr_vector<XIOput>::iterator it;
  int rc, errors = 0;

  /* For each input */
  for (it = xfunction->getInputs()->begin();
      it != xfunction->getInputs()->end(); ++it) {
    /* Validate the communication */
    rc = validateAgentCommunication(&(*it), agent);
    if (rc != 0) {
      printErr("\tfrom input of message: %s\n",
          (*it).getMessageName().c_str());
      errors += rc;
    }
  }

  /* For each output */
  for (it = xfunction->getOutputs()->begin();
      it != xfunction->getOutputs()->end(); ++it) {
    /* Validate the communication */
    rc = validateAgentCommunication(&(*it), agent);
    if (rc != 0) {
      printErr("\tfrom output of message: %s\n",
          (*it).getMessageName().c_str());
      errors += rc;
    }
  }

  return errors;
}

int XMachineValidate::validateAgentFunction(XFunction * xfunction,
    XMachine * agent) {
  int errors = 0;
  std::string name = xfunction->getName();

  /* Process function first */
  processAgentFunction(xfunction, agent->getVariables());

  /* Check name is valid */
  if (!ValidateName::name_is_allowed(name)) {
    printErr("Error: Function name is not valid: %s\n", name.c_str());
    ++errors;
  }

  /* Check current state name is valid */
  if (!ValidateName::name_is_allowed(xfunction->getCurrentState())) {
    printErr("Error: Function current state name is not valid: %s\n",
        xfunction->getCurrentState().c_str());
    ++errors;
  }

  /* Check next state name is valid */
  if (!ValidateName::name_is_allowed(xfunction->getNextState())) {
    printErr("Error: Function next state name is not valid: %s\n",
        xfunction->getNextState().c_str());
    ++errors;
  }

  /* If condition then process and validate */
  if (xfunction->getCondition())
    errors += validateAgentConditionOrFilter(xfunction->getCondition(),
        agent, 0);

  // Validate agent communication
  errors += validateAgentFunctionIOput(xfunction, agent);

  return errors;
}

int XMachineValidate::validateRandomString(XIOput * xioput) {
  if (xioput->getRandomString() == "true") {
    xioput->setRandom(true);
  } else if (xioput->getRandomString() == "false") {
    xioput->setRandom(false);
  } else {
    printErr("Error: input random is not 'true' or 'false': %s\n",
        xioput->getRandomString().c_str());
    return 1;
  }

  return 0;
}

int XMachineValidate::validateAgentCommunication(XIOput * xioput,
    XMachine * agent) {
  int errors = 0;
  boost::ptr_vector<XMessage>::iterator it;
  XMessage * xmessage = 0;

  for (it =  messages_.begin(); it != messages_.end(); ++it)
    if (xioput->getMessageName() == (*it).getName()) xmessage = &(*it);

  /* Check message type */
  if (xmessage == 0) {
    printErr("Error: message name is not valid: %s\n",
        xioput->getMessageName().c_str());
    ++errors;
  }

  /* If filter then process and validate */
  if (xioput->getFilter() != 0) errors += validateAgentConditionOrFilter(
      xioput->getFilter(), agent, xmessage);

  /* If sort then validate */
  if (xioput->isSort()) errors += validateSort(xioput, xmessage);

  /* If random then validate */
  if (xioput->isRandomSet()) errors += validateRandomString(xioput);

  /* Cannot be sorted and random at the same time */
  if (xioput->isSort() && xioput->isRandom()) {
    printErr("Error: input cannot be sorted and random too: %s\n",
        xioput->getMessageName().c_str());
    ++errors;
  }

  return errors;
}

int XMachineValidate::validateAgentConditionOrFilter(XCondition * xcondition,
    XMachine * agent, XMessage * xmessage) {
  int errors = 0;

  errors += xcondition->processSymbols();
  errors += xcondition->validate(agent, xmessage, timeUnits_, xcondition);

  return errors;
}

int XMachineValidate::validateSort(XIOput * xioput, XMessage * xmessage) {
  int errors = 0;

  /* Validate key as a message variable */
  /* Check message is valid */
  if (xmessage != 0) {
    if (!xmessage->validateVariableName(xioput->getSortKey())) {
      printErr("Error: sort key is not a valid message variable: %s\n",
          xioput->getSortKey().c_str());
      ++errors;
    }
    /* If message is not valid */
  } else {
    printErr(
        "Error: cannot validate sort key as the message type is invalid: %s\n",
        xioput->getSortKey().c_str());
    ++errors;
  }
  /* Validate order as ascending or descending */
  if (xioput->getSortOrder() != "ascend" &&
      xioput->getSortOrder() != "descend") {
    printErr("Error: sort order is not 'ascend' or 'descend': %s \n",
        xioput->getSortOrder().c_str());
    ++errors;
  }

  return errors;
}

int XMachineValidate::validateAgentStateGraph(XMachine * agent) {
  // return code and number of errors
  int rc, errors = 0;
  // return error (code and error message)
  std::pair<int, std::string> rerr;

  // Validate single start state
  rc = agent->findStartEndStates();
  if (rc == 1) {
    printErr("Error: %s agent doesn't have a start state\n",
        agent->getName().c_str());
    ++errors;
  } else if (rc == 2) {
    printErr("Error: %s agent has multiple possible start states\n",
        agent->getName().c_str());
    ++errors;
  } else {
    // Generate state graph
    agent->generateStateGraph();
    // Check graph for no cyclic dependencies
    rerr = agent->checkCyclicDependencies();
    if (rerr.first > 0) {
      errors += rerr.first;
      printErr(rerr.second.c_str());
    }
    // Check functions from state with more than one
    // out going function all have conditions
    rerr = agent->checkFunctionConditions();
    if (rerr.first > 0) {
      errors += rerr.first;
      printErr(rerr.second.c_str());
    }
  }
  return errors;
}

}}  // namespace flame::model
