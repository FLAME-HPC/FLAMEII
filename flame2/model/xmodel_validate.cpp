/*!
 * \file flame2/model/xmodel_validate.cpp
 * \author Simon Coakley
 * \date 2012
 * \copyright Copyright (c) 2012 STFC Rutherford Appleton Laboratory
 * \copyright Copyright (c) 2012 University of Sheffield
 * \copyright GNU Lesser General Public License
 * \brief Checks and validates a loaded model
 */
#include <cstdio>
#include <string>
#include <vector>
#include <set>
#include <algorithm>
#include <utility>  // for std::pair
#include <boost/lexical_cast.hpp>
#include <boost/algorithm/string/predicate.hpp>
#include "flame2/config.hpp"
#include "flame2/model/xmodel.hpp"
#include "xmodel_validate.hpp"
#include "xmachine_validate.hpp"
#include "xvariables_validate.hpp"
#include "validate_name.hpp"
#include "printerr.hpp"

namespace flame { namespace model {

XModelValidate::XModelValidate(XModel * m)
: model(m),
  // Create local pointers to model objects
  functionFiles_(m->getFunctionFiles()),
  constants_(m->getConstants()),
  datatypes_(m->getDataTypes()),
  timeUnits_(m->getTimeUnits()),
  agents_(m->getAgents()),
  messages_(m->getMessages()) {}

int XModelValidate::validate() {
  int errors = 0, rc;
  std::vector<std::string>::iterator it;
  boost::ptr_vector<XDataType>::iterator adt_it;
  boost::ptr_vector<XMachine>::iterator a_it;
  boost::ptr_vector<XMessage>::iterator m_it;
  boost::ptr_vector<XMachine>::iterator mit;

  // validate function files
  for (it = functionFiles_->begin(); it != functionFiles_->end(); ++it)
    errors += validateFunctionFile(*it);
  // validate data types
  for (adt_it = datatypes_->begin(); adt_it != datatypes_->end(); ++adt_it)
    if (!(*adt_it).isFundamental()) errors += validateDataType(&(*adt_it));
  // validate model constants
  XVariablesValidate xvariablesValidate(*datatypes_);
  rc = xvariablesValidate.validateVariables(constants_, false);
  if (rc != 0) printErr("\tfrom environment constants.\n");
  errors += rc;
  // validate time units
  errors += validateTimeUnits(timeUnits_);
  // validate agents
  XMachineValidate xmachineValidate(*datatypes_, *messages_, *timeUnits_);
  for (a_it = agents_->begin(); a_it != agents_->end(); ++a_it)
    errors += xmachineValidate.validateAgent(&(*a_it));
  /* Check for duplicate names */
  // ToDo
  /*for (mit = agents_->begin(); mit != agents_->end(); ++mit)
    if (agent->getID() != (*mit).getID() && name == (*mit).getName()) {
      printErr("Error: Duplicate agent name: %s\n", name.c_str());
      ++errors;
    }*/
  // validate messages
  for (m_it = messages_->begin(); m_it != messages_->end(); ++m_it)
    errors += validateMessage(&(*m_it));
  // validate model graph
  validateModelGraph();

  // if errors print out information
  if (errors > 0) printErr("%d error(s) found.\n", errors);

  // return number of errors
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

int XModelValidate::validateFunctionFile(std::string name) {
  int errors = 0;

  /* Name ends in '.c' or '.cpp' */
  if (!(boost::algorithm::ends_with(name, ".cpp"))) {
    printErr("Error: Function file does not end in '.cpp': %s\n",
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
  if (!ValidateName::name_is_allowed(name) || name == "iteration") {
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

int XModelValidate::validateDataType(XDataType * adt) {
  int errors = 0;
  boost::ptr_vector<XDataType>::iterator dit;
  boost::ptr_vector<XVariable>::iterator vit;
  bool dataTypeNameIsValid;
  std::string name = adt->getName();

  // Check name is valid
  dataTypeNameIsValid = ValidateName::name_is_allowed(name);
  if (!dataTypeNameIsValid) {
    printErr("Error: Data type name is not valid: %s\n", name.c_str());
    ++errors;
  }

  // Check there are no duplicates
  for (dit = datatypes_->begin();
      dit != datatypes_->end(); ++dit)
    if (name == (*dit).getName() && adt->getID() != (*dit).getID()) {
      dataTypeNameIsValid = false;
      printErr("Error: Data type already exists: %s\n", name.c_str());
      ++errors;
    }

  // If ADT name is valid
  if (dataTypeNameIsValid) {
    XVariablesValidate xvariablesValidate(*datatypes_);
    errors += xvariablesValidate.validateVariables(adt->getVariables(), true);

    // Check if adt contains dynamic arrays
    for (vit = adt->getVariables()->begin();
        vit != adt->getVariables()->end(); ++vit)
      if ((*vit).holdsDynamicArray()) adt->setHoldsDynamicArray(true);
  }

  if (errors > 0) printErr("\tfrom data type: %s\n", name.c_str());

  return errors;
}

int XModelValidate::validateMessage(XMessage * xmessage) {
  int errors = 0;
  boost::ptr_vector<XMessage>::iterator it;
  std::string name = xmessage->getName();

  /* Check name is valid */
  if (!ValidateName::name_is_allowed(name)) {
    printErr("Error: Message name is not valid: %s\n", name.c_str());
    ++errors;
  }

  /* Check for duplicate names */
  for (it = messages_->begin(); it != messages_->end(); ++it)
    if (xmessage->getID() != (*it).getID() && name == (*it).getName()) {
      printErr("Error: Duplicate message name: %s\n", name.c_str());
      ++errors;
    }

  XVariablesValidate xvariablesValidate(*datatypes_);
  errors += xvariablesValidate.validateVariables(
      xmessage->getVariables(), false);

  if (errors > 0) printErr("\tfrom message: %s\n", name.c_str());

  return errors;
}

void XModelValidate::validateModelGraph() {
  // create model state graph
  model->generateStateGraph();
}

}}  // namespace flame::model
