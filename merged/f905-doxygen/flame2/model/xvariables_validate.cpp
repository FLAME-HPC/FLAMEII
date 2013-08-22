/*!
 * \file flame2/model/xvariables_validate.cpp
 * \author Simon Coakley
 * \date 2013
 * \copyright Copyright (c) 2013 STFC Rutherford Appleton Laboratory
 * \copyright Copyright (c) 2013 University of Sheffield
 * \copyright GNU Lesser General Public License
 * \brief XVariablesValidate: holds variables validator
 */
#include <string>
#include <vector>
#include <boost/lexical_cast.hpp>
#include "flame2/config.hpp"
#include "xvariables_validate.hpp"
#include "validate_name.hpp"
#include "printerr.hpp"

namespace flame { namespace model {

XVariablesValidate::XVariablesValidate(
    const boost::ptr_vector<XDataType>& datatypes)
  : datatypes_(datatypes) {}

int XVariablesValidate::validateVariables(
    boost::ptr_vector<XVariable> * variables,
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

bool XVariablesValidate::castStringToInt(std::string * str, int * i) {
  try {
    *i = boost::lexical_cast<int>(*str);
  }
  catch(const boost::bad_lexical_cast&) {
    return false;
  }
  return true;
}

int XVariablesValidate::processVariableStaticArray(XVariable * variable) {
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

int XVariablesValidate::processVariable(XVariable * variable) {
  int errors = 0;
  boost::ptr_vector<XDataType>::const_iterator it;

  /* Process variables for any arrays defined */
  processVariableDynamicArray(variable);
  errors += processVariableStaticArray(variable);

  /* Check if data type is a user defined data type */
  for (it = datatypes_.begin(); it != datatypes_.end(); ++it) {
    if (variable->getType() == (*it).getName() &&
        (*it).isFundamental() == false) {
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

int XVariablesValidate::processVariables(
    boost::ptr_vector<XVariable> * variables) {
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

void XVariablesValidate::validateVariableName(XVariable * v, int * errors,
    boost::ptr_vector<XVariable> * variables) {
  boost::ptr_vector<XVariable>::iterator it;
  /* Check names are valid */
  if (!ValidateName::name_is_allowed(v->getName())) {
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

void XVariablesValidate::validateVariableType(XVariable * v, int * errors,
    bool allowDyamicArrays) {
  std::vector<std::string>::iterator it;
  boost::ptr_vector<XDataType>::const_iterator dit;
  bool datatypeFound = false;

  /* Check for valid types */
  for (dit = datatypes_.begin(); dit != datatypes_.end(); ++dit)
    if (v->getType() == (*dit).getName()) datatypeFound = true;
  /* If valid data type not found */
  if (datatypeFound == false) {
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

void XVariablesValidate::processVariableDynamicArray(XVariable * variable) {
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

} }  // namespace flame::model
