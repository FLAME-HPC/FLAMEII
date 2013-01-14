/*!
 * \file flame2/model/xmodel_check.cpp
 * \author Simon Coakley
 * \date 2012
 * \copyright Copyright (c) 2012 STFC Rutherford Appleton Laboratory
 * \copyright Copyright (c) 2012 University of Sheffield
 * \copyright GNU Lesser General Public License
 * \brief Checks and validates a loaded model
 */
#ifndef TESTBUILD
#include <cstdarg>
#endif
#include <cstdio>
#include <string>
#include <vector>
#include <set>
#include <algorithm>
#include <boost/lexical_cast.hpp>
#include <boost/algorithm/string/predicate.hpp>
#include "flame2/config.hpp"
#include "flame2/model/xmodel.hpp"
#include "xmodel_validate.hpp"

void printErr(const char *format, ...) {
  // Print message to stderr
#ifndef TESTBUILD
  va_list arg;
  va_start(arg, format);
  std::vfprintf(stderr, format, arg);
  va_end(arg);
#endif
}

namespace flame { namespace model {

XModelValidate::XModelValidate(XModel * m)
: model(m),
  // Create local pointers to model objects
  functionFiles_(m->getFunctionFiles()),
  constants_(m->getConstants()),
  adts_(m->getADTs()),
  timeUnits_(m->getTimeUnits()),
  agents_(m->getAgents()),
  messages_(m->getMessages()) {}

/*!
 * \brief Validate the model
 * \return The number of errors found
 * \todo Check condition op IN has valid variables (if implemented).
 * \todo Check for no agents (if needed).
 * \todo Remove agents with no memory variables (if needed).
 * Detailed description.
 */
int XModelValidate::validate() {
  int errors = 0, rc;
  std::vector<std::string>::iterator it;
  boost::ptr_vector<XADT>::iterator adt_it;
  boost::ptr_vector<XMachine>::iterator a_it;
  boost::ptr_vector<XMessage>::iterator m_it;

  /* Validate function files */
  for (it = functionFiles_->begin(); it != functionFiles_->end(); ++it)
    errors += validateFunctionFile(*it);
  /* Validate data types */
  for (adt_it = adts_->begin(); adt_it != adts_->end(); ++adt_it)
    errors += validateADT(&(*adt_it));
  /* Validate model constants */
  rc = validateVariables(model->getConstants(), false);
  if (rc != 0) printErr("\tfrom environment constants.\n");
  errors += rc;
  /* Validate time units */
  errors += validateTimeUnits(model->getTimeUnits());
  /* Validate agents */
  for (a_it = agents_->begin(); a_it != agents_->end(); ++a_it)
    errors += validateAgent(&(*a_it));
  /* Validate messages */
  for (m_it = messages_->begin(); m_it != messages_->end(); ++m_it)
    errors += validateMessage(&(*m_it));

  /* If errors print out information */
  if (errors > 0) printErr("%d error(s) found.\n", errors);

  /* Return number of errors */
  return errors;
}

int XModelValidate::validateTimeUnits(
    boost::ptr_vector<XTimeUnit> * timeUnits) {
  int errors = 0, rc;
  boost::ptr_vector<XTimeUnit>::iterator it;

  // Process time unit (unit and period)
  for (it = timeUnits->begin(); it != timeUnits->end(); ++it) {
    rc = processTimeUnit(&(*it));
    if (rc != 0) printErr("\tfrom time unit: %s\n", (*it).getName().c_str());
    errors += rc;
  }

  // Validate time unit
  for (it = timeUnits->begin(); it != timeUnits->end(); ++it) {
    rc = validateTimeUnit(&(*it));
    if (rc != 0) printErr("\tfrom time unit: %s\n", (*it).getName().c_str());
    errors += rc;
  }

  // Return number of errors
  return errors;
}

int XModelValidate::validateAgentStateGraph(XMachine * agent) {
  int rc, errors = 0;

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
    errors += agent->generateStateGraph();
    // Check graph for no cyclic dependencies
    errors += agent->checkCyclicDependencies();
    // Check functions from state with more than one
    // out going function all have conditions
    errors += agent->checkFunctionConditions();
  }
  return errors;
}

int XModelValidate::validateAgent(XMachine * agent) {
  int rc, errors = 0;
  boost::ptr_vector<XMachine>::iterator mit;
  boost::ptr_vector<XFunction>::iterator fit;
  std::string name = agent->getName();

  /* Check name is valid */
  if (!name_is_allowed(name)) {
    printErr("Error: Agent name is not valid: %s\n", name.c_str());
    ++errors;
  }

  /* Check for duplicate names */
  for (mit = agents_->begin(); mit != agents_->end(); ++mit)
    if (agent->getID() != (*mit).getID() && name == (*mit).getName()) {
      printErr("Error: Duplicate agent name: %s\n", name.c_str());
      ++errors;
    }

  /* Validate variables */
  errors += validateVariables(agent->getVariables(), true);

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

void XModelValidate::processVariableDynamicArray(XVariable * variable) {
  /* Handle dynamic arrays in variable type */
  /* If the type can end in _array */
  if (variable->getType().size() > 6) {
    /* If the type ends in _array */
    if (variable->getType().compare(
        variable->getType().size()-6, 6, "_array") == 0) {
      /* Set dynamic array to be true */
      variable->setIsDynamicArray(true);
      /* Set that variable holds dynamic array */
      variable->setHoldsDynamicArray(true);
      /* Remove _array from end of type */
      variable->setType(
          variable->getType().erase(
              variable->getType().size()-6, 6));
    }
  }
}

bool castStringToInt(std::string * str, int * i) {
  try {
    *i = boost::lexical_cast<int>(*str);
  }
  catch(const boost::bad_lexical_cast&) {
    return false;
  }
  return true;
}

int XModelValidate::processVariableStaticArray(XVariable * variable) {
  int errors = 0;
  /* Handle static arrays in variable name */
  int arraySize;
  /* Find brackets */
  size_t positionOfOpenBracket = variable->getName().find("[");
  size_t positionOfCloseBracket = variable->getName().find("]");
  /* If brackets found */
  if (positionOfOpenBracket != std::string::npos &&
      positionOfCloseBracket != std::string::npos) {
    /* If close bracket is at the end of the name */
    if (positionOfCloseBracket == variable->getName().size()-1) {
      /* Set static array to be true */
      variable->setIsStaticArray(true);
      /* Convert array size number to integer */
      std::string arraySizeString =
          variable->getName().substr(positionOfOpenBracket+1,
              positionOfCloseBracket-positionOfOpenBracket-1);
      /* Try and cast and if successful */
      if (castStringToInt(&arraySizeString, &arraySize)) {
        if (arraySize < 1) {
          printErr("Error: Static array size is not valid: %d\n", arraySize);
          ++errors;
        }
        /* Set successful array size integer */
        variable->setStaticArraySize(arraySize);
        /* If cast not successful */
      } else {
        printErr("Error: Static array number not an integer: %s\n",
            arraySizeString.c_str());
        ++errors;
      }

      /* Remove bracket from name */
      variable->setName(variable->getName().erase(positionOfOpenBracket));
    }
  }
  return errors;
}

int XModelValidate::processVariable(XVariable * variable) {
  int errors = 0;
  boost::ptr_vector<XADT>::iterator it;

  /* Process variables for any arrays defined */
  processVariableDynamicArray(variable);
  errors += processVariableStaticArray(variable);

  /* Check if data type is a user defined data type */
  for (it = adts_->begin(); it != adts_->end(); ++it) {
    if (variable->getType() == (*it).getName()) {
      variable->setHasADTType(true);
      /* If the ADT holds dynamic arrays then set this variable */
      if ((*it).holdsDynamicArray()) variable->setHoldsDynamicArray(true);
    }
  }

  /* Validate constant if set */
  if (variable->isConstantSet()) {
    if (variable->getConstantString() == "true") {
      variable->setConstant(true);
    } else if (variable->getConstantString() == "false") {
      variable->setConstant(false);
    } else {
      /* If constant value is not true or false */
      printErr("Error: variable constant value is not 'true' or 'false': %s\n",
          variable->getConstantString().c_str());
      ++errors;
    }
  }

  return errors;
}

int XModelValidate::processVariables(boost::ptr_vector<XVariable> * variables) {
  int rc, errors = 0;
  boost::ptr_vector<XVariable>::iterator it;

  /* Handle dynamic arrays in variable type */
  for (it = variables->begin(); it != variables->end(); ++it) {
    rc = processVariable(&(*it));
    if (rc != 0) printErr("\tfrom variable: %s %s\n",
        (*it).getType().c_str(), (*it).getName().c_str());
    errors += rc;
  }

  return errors;
}

bool XModelValidate::variableExists(std::string name,
    boost::ptr_vector<XVariable> * variables) {
  boost::ptr_vector<XVariable>::iterator vit;

  for (vit = variables->begin(); vit != variables->end(); ++vit)
    if (name == (*vit).getName()) return true;
  return false;
}

int XModelValidate::processMemoryAccessVariable(std::string name,
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

int XModelValidate::processAgentFunction(XFunction * function,
    boost::ptr_vector<XVariable> * variables) {
  std::vector<std::string>::iterator variable;
  boost::ptr_vector<XVariable>::iterator variable2;
  std::set<std::string> usedVariables;
  int errors = 0;

  // If memory access information was not given set all memory
  // variable access as being read write.
  if (!function->getMemoryAccessInfoAvailable()) {
    function->setMemoryAccessInfoAvailable(true);
    for (variable2 = variables->begin();
        variable2 != variables->end(); ++variable2) {
      function->addReadWriteVariable((*variable2).getName());
    }
    // Else check memory access variables are valid
  } else {
    // Check variable names are valid variables in memory
    for (variable = function->getReadOnlyVariables()->begin();
        variable != function->getReadOnlyVariables()->end();
        ++variable)
      errors += processMemoryAccessVariable(
          (*variable), variables, &usedVariables);
    for (variable = function->getReadWriteVariables()->begin();
        variable != function->getReadWriteVariables()->end();
        ++variable)
      errors += processMemoryAccessVariable(
          (*variable), variables, &usedVariables);
  }

  return errors;
}

void XModelValidate::validateVariableName(XVariable * v, int * errors,
    boost::ptr_vector<XVariable> * variables) {
  boost::ptr_vector<XVariable>::iterator it;
  /* Check names are valid */
  if (!name_is_allowed(v->getName())) {
    printErr("Error: Variable name is not valid: %s\n",
        v->getName().c_str());
    ++(*errors);
  }
  /* Check for duplicate names */
  for (it = variables->begin(); it != variables->end(); ++it) {
    /* Check names are equal and not same variable */
    if (v != &(*it) &&
        v->getName() == (*it).getName()) {
      printErr("Error: Duplicate variable name: %s\n",
          v->getName().c_str());
      ++(*errors);
    }
  }
}

void XModelValidate::validateVariableType(XVariable * v, int * errors,
    bool allowDyamicArrays) {
  std::vector<std::string>::iterator it;
  /* Check for valid types */
  it = std::find(model->getAllowedDataTypes()->begin(),
      model->getAllowedDataTypes()->end(), v->getType());
  /* If valid data type not found */
  if (it == model->getAllowedDataTypes()->end()) {
    printErr("Error: Data type: '%s' not valid for variable name: %s %s\n",
        v->getType().c_str(), v->getName().c_str());
    ++(*errors);
  }

  /* Check for allowed dynamic arrays */
  /* If dynamic arrays not allowed and variable holds a dynamic array */
  if (!allowDyamicArrays && v->holdsDynamicArray()) {
    if (v->isDynamicArray())
      printErr("Error: Dynamic array not allowed: %s_array %s\n",
          v->getType().c_str(), v->getName().c_str());
    else
      printErr("Error: Dynamic array (in data type) not allowed: %s %s\n",
          v->getType().c_str(), v->getName().c_str());
    ++(*errors);
  }
}

int XModelValidate::validateVariables(boost::ptr_vector<XVariable> * variables,
    bool allowDyamicArrays) {
  int errors = 0;
  boost::ptr_vector<XVariable>::iterator it;

  /* Process variables first */
  errors += processVariables(variables);

  /* For each variable */
  for (it = variables->begin(); it != variables->end(); ++it) {
    /* Validate variable name */
    validateVariableName(&(*it), &errors, variables);
    /* Validate variable type */
    validateVariableType(&(*it), &errors, allowDyamicArrays);
  }

  return errors;
}

int XModelValidate::validateFunctionFile(std::string name) {
  int errors = 0;

  /* Name ends in '.c' or '.cpp' */
  if (!(boost::algorithm::ends_with(name, ".c") ||
      boost::algorithm::ends_with(name, ".cpp"))) {
    printErr("Error: Function file does not end in '.c': %s\n",
        name.c_str());
    ++errors;
  }

  return errors;
}

int XModelValidate::processTimeUnitPeriod(XTimeUnit * timeUnit) {
  int errors = 0;
  bool castSuccessful = true;
  int period;
  /* Process period and validate */
  try {
    /* Try and cast to integer */
    period = boost::lexical_cast<int>(timeUnit->getPeriodString());
  } catch(const boost::bad_lexical_cast&) {
    printErr("Error: Period number not an integer: %s\n",
        timeUnit->getPeriodString().c_str());
    ++errors;
    castSuccessful = false;
  }
  /* If cast was successful */
  if (castSuccessful) {
    if (period < 1) {
      printErr("Error: Period value is not valid: %d\n", period);
      ++errors;
    }
    /* Set successful array size integer */
    timeUnit->setPeriod(period);
  }
  return errors;
}

int XModelValidate::processTimeUnitUnit(XTimeUnit * timeUnit) {
  int errors = 0;
  boost::ptr_vector<XTimeUnit>::iterator it;
  bool unitIsValid = false;
  /* Unit can either be 'iteration' */
  if (timeUnit->getUnit() == "iteration") unitIsValid = true;
  /* Or unit can be another time unit name */
  for (it = timeUnits_->begin(); it != timeUnits_->end(); ++it)
    if (timeUnit->getName() != (*it).getName() &&
        timeUnit->getUnit() == (*it).getName())
      unitIsValid = true;
  if (!unitIsValid) {
    printErr("Error: Time unit unit is not valid: %s\n",
        timeUnit->getUnit().c_str());
    ++errors;
  }
  return errors;
}

int XModelValidate::validateTimeUnit(XTimeUnit * tU) {
  boost::ptr_vector<XTimeUnit>::iterator it;
  int errors = 0;
  std::string name = tU->getName();

  /* Check name is valid */
  if (!name_is_allowed(name) || name == "iteration") {
    printErr("Error: Time unit name is not valid: %s\n", name.c_str());
    ++errors;
  }

  /* Check for duplicate names */
  for (it = model->getTimeUnits()->begin();
      it != model->getTimeUnits()->end(); ++it)
    // If time units are not the same check names
    if (tU->getID() != (*it).getID() && name == (*it).getName()) {
      printErr("Error: Duplicate time unit name: %s\n", name.c_str());
      ++errors;
    }

  return errors;
}

int XModelValidate::processTimeUnit(XTimeUnit * timeUnit) {
  int errors = 0;

  errors += processTimeUnitUnit(timeUnit);
  errors += processTimeUnitPeriod(timeUnit);

  return errors;
}

int XModelValidate::validateADT(XADT * adt) {
  int errors = 0;
  std::vector<std::string>::iterator dit;
  boost::ptr_vector<XVariable>::iterator vit;
  bool dataTypeNameIsValid;
  std::string name = adt->getName();

  // Check name is valid
  dataTypeNameIsValid = name_is_allowed(name);
  if (!dataTypeNameIsValid) {
    printErr("Error: Data type name is not valid: %s\n", name.c_str());
    ++errors;
  }

  // Check ADT name is not already a valid data type
  for (dit = model->getAllowedDataTypes()->begin();
      dit != model->getAllowedDataTypes()->end(); ++dit)
    if (name == (*dit)) {
      dataTypeNameIsValid = false;
      printErr("Error: Data type already exists: %s\n", name.c_str());
      ++errors;
    }

  // If ADT name is valid
  if (dataTypeNameIsValid) {
    // Add data type to list of model allowed data types
    model->addAllowedDataType(name);

    errors += validateVariables(adt->getVariables(), true);

    // Check if adt contains dynamic arrays
    for (vit = adt->getVariables()->begin();
        vit != adt->getVariables()->end(); ++vit)
      if ((*vit).holdsDynamicArray()) adt->setHoldsDynamicArray(true);
  }

  if (errors > 0) printErr("\tfrom data type: %s\n", name.c_str());

  return errors;
}

int XModelValidate::validateAgentFunctionIOput(XFunction * xfunction,
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

int XModelValidate::validateAgentFunction(XFunction * xfunction,
    XMachine * agent) {
  int errors = 0;
  std::string name = xfunction->getName();

  /* Process function first */
  processAgentFunction(xfunction, agent->getVariables());

  /* Check name is valid */
  if (!name_is_allowed(name)) {
    printErr("Error: Function name is not valid: %s\n", name.c_str());
    ++errors;
  }

  /* Check current state name is valid */
  if (!name_is_allowed(xfunction->getCurrentState())) {
    printErr("Error: Function current state name is not valid: %s\n",
        xfunction->getCurrentState().c_str());
    ++errors;
  }

  /* Check next state name is valid */
  if (!name_is_allowed(xfunction->getNextState())) {
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

int XModelValidate::validateRandomString(XIOput * xioput) {
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

int XModelValidate::validateAgentCommunication(XIOput * xioput,
    XMachine * agent) {
  int errors = 0;
  XMessage * xmessage = model->getMessage(xioput->getMessageName());

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

int XModelValidate::validateAgentConditionOrFilter(XCondition * xcondition,
    XMachine * agent, XMessage * xmessage) {
  int errors = 0;

  errors += xcondition->processSymbols();
  errors += xcondition->validate(agent, xmessage, model, xcondition);

  return errors;
}

int XModelValidate::validateSort(XIOput * xioput, XMessage * xmessage) {
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

int XModelValidate::validateMessage(XMessage * xmessage) {
  int errors = 0;
  boost::ptr_vector<XMessage>::iterator it;
  std::string name = xmessage->getName();

  /* Check name is valid */
  if (!name_is_allowed(name)) {
    printErr("Error: Message name is not valid: %s\n", name.c_str());
    ++errors;
  }

  /* Check for duplicate names */
  for (it = messages_->begin(); it != messages_->end(); ++it)
    if (xmessage->getID() != (*it).getID() && name == (*it).getName()) {
      printErr("Error: Duplicate message name: %s\n", name.c_str());
      ++errors;
    }

  errors += validateVariables(xmessage->getVariables(), false);

  if (errors > 0) printErr("\tfrom message: %s\n", name.c_str());

  return errors;
}

bool char_is_disallowed(char c) {
  return !(isalnum(c) || c == '_' || c == '-');
}

/*!
 * \brief Validates a name string
 * \param[in] name The string to validate
 * \return Boolean result
 * The iterator tries to find the first element that satisfies the predicate.
 * If no disallowed character is found then you end with name.end() which is
 * taken to be success and true is returned.
 */
bool XModelValidate::name_is_allowed(std::string name) {
  return std::find_if(name.begin(), name.end(),
      char_is_disallowed) == name.end();
}

}}  // namespace flame::model
