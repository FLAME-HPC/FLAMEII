/*!
 * \file flame2/model/xmodel_check.cpp
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
#include <boost/lexical_cast.hpp>
#include <boost/algorithm/string/predicate.hpp>
#include "flame2/config.hpp"
#include "flame2/model/xmodel.hpp"
#include "xmodel_validate.hpp"

void printErr(std::string message) {
    // Print message to stderr
#ifndef TESTBUILD
    std::fprintf(stderr, "%s\n", message.c_str());
#endif
}

namespace flame { namespace model {

XModelValidate::XModelValidate(XModel * m) {
    model = m;
}

XModelValidate::~XModelValidate() {}

/*!
 * \brief Validate the model
 * \return The number of errors found
 * \todo Check condition op IN has valid variables (if implemented).
 * \todo Check for no agents (if needed).
 * \todo Remove agents with no memory variables (if needed).
 * Detailed description.
 */
int XModelValidate::validate() {
    int rc;
    int errors = 0;

    /* Validate function files */
    errors += validateFunctionFiles(*model->getFunctionFiles());
    /* Validate data types */
    errors += validateDataTypes(*model->getADTs(), model);
    /* Validate model constants */
    rc = validateVariables(model->getConstants(), model, false);
    if (rc != 0) {
        printErr("       from environment constants.");
        errors += rc;
    }
    /* Validate time units */
    errors += validateTimeUnits(*model->getTimeUnits(), model);
    /* Validate agents */
    errors += validateAgents(*model->getAgents(), model);
    /* Validate messages */
    errors += validateMessages(*model->getMessages(), model);

    /* If errors print out information */
    if (errors > 0) {
        printErr(std::string(boost::lexical_cast<std::string>(errors) +
                " error(s) found."));
    }

    /* Return number of errors */
    return errors;
}

int XModelValidate::validateFunctionFiles(std::vector<std::string> names) {
    int errors = 0;
    unsigned int ii;
    for (ii = 0; ii < names.size(); ii++) {
        errors += validateFunctionFile(names.at(ii));
    }
    return errors;
}

int XModelValidate::validateDataTypes(std::vector<XADT*> adts, XModel * model) {
    int errors = 0;
    unsigned int ii;
    int rc;
    for (ii = 0; ii < adts.size(); ii++) {
        rc = validateADT(adts.at(ii), model);
        if (rc != 0) {
            printErr(std::string("       from data type: ") +
                adts.at(ii)->getName());
            errors += rc;
        }
    }
    return errors;
}

int XModelValidate::validateTimeUnits(
        std::vector<XTimeUnit*> timeUnits, XModel * model) {
    int errors = 0;
    unsigned int ii;
    int rc;

    // Process time unit (unit and period)
    for (ii = 0; ii < timeUnits.size(); ii++) {
        rc = processTimeUnit(timeUnits.at(ii));
        if (rc != 0) {
            printErr(std::string("       from time unit: ") +
                timeUnits.at(ii)->getName());
            errors++;
        }
    }

    for (ii = 0; ii < timeUnits.size(); ii++) {
        rc = validateTimeUnit(timeUnits.at(ii), model);
        if (rc != 0) {
            printErr(std::string("       from time unit: ") +
                timeUnits.at(ii)->getName());
            errors++;
        }
    }

    return errors;
}

int XModelValidate::validateAgents(std::vector<XMachine*> agents,
        XModel * model) {
    int errors = 0;
    unsigned int ii;
    int rc;
    for (ii = 0; ii < agents.size(); ii++) {
        rc = validateAgent(agents.at(ii), model);
        if (rc != 0) {
            printErr(std::string("       from agent: ") +
                agents.at(ii)->getName());
            errors += rc;
        }
    }
    return errors;
}

int XModelValidate::validateMessages(std::vector<XMessage*> messages,
        XModel * model) {
    int errors = 0;
    unsigned int ii;
    int rc;
    for (ii = 0; ii < messages.size(); ii++) {
        rc = validateMessage(messages.at(ii), model);
        if (rc != 0) {
            printErr(std::string("       from message: ") +
                messages.at(ii)->getName());
            errors += rc;
        }
    }
    return errors;
}

int XModelValidate::validateAgentStateGraph(XMachine * agent) {
    int rc, errors = 0;
    // Validate single start state
    rc = agent->findStartEndStates();
    if (rc != 0) {
        errors += rc;
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

int XModelValidate::validateAgent(XMachine * agent, XModel * model) {
    int rc, errors = 0;
    unsigned int ii;

    /* Check name is valid */
    if (!name_is_allowed(agent->getName())) {
        printErr(std::string("Error: Agent name is not valid: ") +
            agent->getName());
        errors++;
    }

    /* Check for duplicate names */
    for (ii = 0; ii < model->getAgents()->size(); ii++)
        if (agent != model->getAgents()->at(ii) &&
                agent->getName() == model->getAgents()->at(ii)->getName()) {
            printErr(std::string("Error: Duplicate agent name: ") +
                agent->getName());
            errors++;
        }

    /* Validate variables */
    errors += validateVariables(agent->getVariables(), model, true);

    /* Validate agent functions */
    for (ii = 0; ii < agent->getFunctions()->size(); ii++) {
        rc = validateAgentFunction(agent->getFunctions()->at(ii),
                agent, model);
        if (rc != 0) { printErr(std::string("       from function: ") +
                agent->getFunctions()->at(ii)->getName());
            errors += rc;
        }
    }

    // Validate agent state graph
    errors += validateAgentStateGraph(agent);

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
                if (arraySize < 1) { printErr(std::string(
                        "Error: Static array size is not valid: ") +
                        boost::lexical_cast<std::string>(arraySize));
                    errors++;
                }
                /* Set successful array size integer */
                variable->setStaticArraySize(arraySize);
            /* If cast not successful */
            } else { printErr(std::string(
                    "Error: Static array number not an integer: ") +
                    arraySizeString);
                errors++;
            }

            /* Remove bracket from name */
            variable->setName(variable->getName().erase(positionOfOpenBracket));
        }
    }
    return errors;
}

int XModelValidate::processVariable(XVariable * variable,
        XModel * model) {
    int errors = 0;
    unsigned int ii;

    /* Process variables for any arrays defined */
    processVariableDynamicArray(variable);
    errors += processVariableStaticArray(variable);

    /* Check if data type is a user defined data type */
    for (ii = 0; ii < model->getADTs()->size(); ii++) {
        if (variable->getType() == model->getADTs()->at(ii)->getName()) {
            variable->setHasADTType(true);
            /* If the ADT holds dynamic arrays then set this variable */
            if (model->getADTs()->at(ii)->holdsDynamicArray())
                variable->setHoldsDynamicArray(true);
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
            printErr(std::string(
            "Error: variable constant value is not 'true' or 'false': ") +
                variable->getConstantString());
            errors++;
        }
    }

    return errors;
}

int XModelValidate::processVariables(std::vector<XVariable*> * variables,
        XModel * model) {
    int rc, errors = 0;
    unsigned int ii;

    /* Handle dynamic arrays in variable type */
    for (ii = 0; ii < variables->size(); ii++) {
        rc = processVariable(variables->at(ii), model);
        if (rc != 0) {
            printErr(std::string(
                "       from variable: ") +
                variables->at(ii)->getType() +
                std::string(" ") +
                variables->at(ii)->getName());
            errors += rc;
        }
    }

    return errors;
}

bool XModelValidate::variableExists(std::string name,
        std::vector<XVariable*> * variables) {
    std::vector<XVariable*>::iterator vit;

    for (vit = variables->begin(); vit != variables->end(); vit++)
            if (name == (*vit)->getName()) return true;
    return false;
}

int XModelValidate::processMemoryAccessVariable(std::string name,
        std::vector<XVariable*> * variables,
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
            printErr(std::string(
                "Error: Memory access variable name is a duplicate: ") +
                name);
            errors++;
        }

    } else {
        // If variable does not exist
        printErr(std::string(
            "Error: Memory access variable name is not valid: ") +
            name);
        errors++;
    }

    return errors;
}

int XModelValidate::processAgentFunction(XFunction * function,
        std::vector<XVariable*> * variables) {
    std::vector<std::string>::iterator variable;
    std::vector<XVariable*>::iterator variable2;
    std::set<std::string> usedVariables;
    int errors = 0;

    // If memory access information was not given set all memory
    // variable access as being read write.
    if (!function->getMemoryAccessInfoAvailable()) {
        for (variable2 = variables->begin();
                variable2 != variables->end(); ++variable2) {
            function->addReadWriteVariable((*variable2)->getName());
        }
    // Else check memory access variables are valid
    } else {
        // Check variable names are valid variables in memory
        for (variable = function->getReadOnlyVariables()->begin();
                variable != function->getReadOnlyVariables()->end();
                variable++)
            errors += processMemoryAccessVariable(
                    (*variable), variables, &usedVariables);
        for (variable = function->getReadWriteVariables()->begin();
                variable != function->getReadWriteVariables()->end();
                variable++)
            errors += processMemoryAccessVariable(
                    (*variable), variables, &usedVariables);
    }

    return errors;
}

void XModelValidate::validateVariableName(XVariable * v, int * errors,
        std::vector<XVariable*> * variables) {
    unsigned int jj;
    /* Check names are valid */
    if (!name_is_allowed(v->getName())) {
        printErr(std::string(
            "Error: Variable name is not valid: ") +
            v->getName());
        (*errors)++;
    }
    /* Check for duplicate names */
    for (jj = 0; jj < variables->size(); jj++) {
        /* Check names are equal and not same variable */
        if (v != variables->at(jj) &&
            v->getName() == variables->at(jj)->getName()) {
            printErr(std::string(
                "Error: Duplicate variable name: ") +
                v->getName());
            (*errors)++;
        }
    }
}

void XModelValidate::validateVariableType(XVariable * v, int * errors,
        XModel * model, bool allowDyamicArrays) {
    unsigned int jj;
    bool foundValidDataType = false;
    /* Check for valid types */
    for (jj = 0; jj < model->getAllowedDataTypes()->size(); jj++)
        if (v->getType() == model->getAllowedDataTypes()->at(jj))
            foundValidDataType = true;
    /* If valid data type not found */
    if (!foundValidDataType) {
        printErr(std::string(
            "Error: Data type: '%s' not valid for variable name: ") +
            v->getType() +
            std::string(" ") +
            v->getName());
        (*errors)++;
    }

    /* Check for allowed dynamic arrays */
    /* If dynamic arrays not allowed and variable holds a dynamic array */
    if (!allowDyamicArrays && v->holdsDynamicArray()) {
        if (v->isDynamicArray())
            printErr(std::string(
                "Error: Dynamic array not allowed: ") +
                    v->getType() +
                    std::string("_array ") +
                    v->getName());
        else
            printErr(std::string(
        "Error: Dynamic array (in data type) not allowed: ") +
                v->getType() +
                std::string(" ") +
                v->getName());
        (*errors)++;
    }
}

int XModelValidate::validateVariables(std::vector<XVariable*> * variables,
        XModel * model, bool allowDyamicArrays) {
    int rc, errors = 0;
    unsigned int ii;

    /* Process variables first */
    rc = processVariables(variables, model);
    errors += rc;

    /* For each variable */
    for (ii = 0; ii < variables->size(); ii++) {
        /* Validate variable name */
        validateVariableName(variables->at(ii), &errors, variables);
        /* Validate variable type */
        validateVariableType(variables->at(ii), &errors,
                model, allowDyamicArrays);
    }

    return errors;
}

int XModelValidate::validateFunctionFile(std::string name) {
    int errors = 0;

    /* Name ends in '.c' */
    if (!boost::algorithm::ends_with(name, ".c")) {
        printErr(std::string(
            "Error: Function file does not end in '.c': ") +
            name);
        errors++;
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
        printErr(std::string("Error: Period number not an integer: ") +
            timeUnit->getPeriodString());
        errors++;
        castSuccessful = false;
    }
    /* If cast was successful */
    if (castSuccessful) {
        if (period < 1) {
            printErr(std::string("Error: Period value is not valid: ") +
                boost::lexical_cast<std::string>(period));
            errors++;
        }
        /* Set successful array size integer */
        timeUnit->setPeriod(period);
    }
    return errors;
}

int XModelValidate::processTimeUnitUnit(XTimeUnit * timeUnit, XModel * model) {
    int errors = 0;
    unsigned int ii;
    bool unitIsValid = false;
    /* Unit can either be 'iteration' */
    if (timeUnit->getUnit() == "iteration") unitIsValid = true;
    /* Or unit can be another time unit name */
    for (ii = 0; ii < model->getTimeUnits()->size(); ii++)
        if (timeUnit != model->getTimeUnits()->at(ii) &&
                timeUnit->getUnit() == model->getTimeUnits()->at(ii)->getName())
            unitIsValid = true;
    if (!unitIsValid) {
        printErr(std::string(
                "Error: Time unit unit is not valid: ") +
                timeUnit->getUnit());
            errors++;
    }
    return errors;
}

int XModelValidate::validateTimeUnit(XTimeUnit * timeUnit, XModel * model) {
    std::vector<XTimeUnit*>::iterator it;

    /* Check name is valid */
    if (!name_is_allowed(timeUnit->getName())) {
        printErr(std::string(
            "Error: Time unit name is not valid: ") +
            timeUnit->getName());
        return 1;
    }

    /* Check that name is not iteration */
    if (timeUnit->getName() == "iteration") {
        printErr(std::string("Error: Time unit name cannot be 'iteration'"));
        return 1;
    }

    /* Check for
     * duplicate names */
    for (it = model->getTimeUnits()->begin();
            it != model->getTimeUnits()->end(); it++) {
        // If time units are not the same check names
        if (timeUnit != (*it) &&
            timeUnit->getName() == (*it)->getName()) {
            // If the time unit is an exact duplicate then delete
            if (timeUnit->getPeriod() ==
                    (*it)->getPeriod() &&
                    timeUnit->getUnit() == (*it)->getUnit()) {
                // Remove exact duplicate
                std::vector<XTimeUnit*>::iterator tit =
                    std::find(model->getTimeUnits()->begin(),
                        model->getTimeUnits()->end(), timeUnit);
                delete (*tit);
                model->getTimeUnits()->erase(tit);
                return 0;
            } else {
                printErr(std::string("Error: Duplicate time unit name: ") +
                    timeUnit->getName());
                return 1;
            }
        }
    }

    return 0;
}

int XModelValidate::processTimeUnit(XTimeUnit * timeUnit) {
    int errors = 0;

    errors += processTimeUnitUnit(timeUnit, model);
    errors += processTimeUnitPeriod(timeUnit);

    return errors;
}

int XModelValidate::validateADT(XADT * adt, XModel * model) {
    int errors = 0;
    unsigned int jj;
    bool dataTypeNameIsValid;

    /* Check name is valid */
    dataTypeNameIsValid = name_is_allowed(adt->getName());
    if (!dataTypeNameIsValid) { printErr(std::string(
            "Error: Data type name is not valid: ") +
            adt->getName());
        errors++;
    }

    /* Check ADT name is not already a valid data type */
    for (jj = 0; jj < model->getAllowedDataTypes()->size(); jj++)
        if (adt->getName() == model->getAllowedDataTypes()->at(jj)) {
            dataTypeNameIsValid = false;
            printErr(std::string(
                "Error: Data type already exists: ") +
                adt->getName());
            errors++;
        }

    if (dataTypeNameIsValid) {
        /* Add data type to list of model allowed data types */
        model->addAllowedDataType(adt->getName());

        errors += validateVariables(adt->getVariables(), model, true);

        /* Check if adt contains dynamic arrays */
        for (jj = 0; jj < adt->getVariables()->size(); jj++)
            if (adt->getVariables()->at(jj)->holdsDynamicArray())
                adt->setHoldsDynamicArray(true);
    }

    return errors;
}

int XModelValidate::validateAgentFunctionIOput(XFunction * xfunction,
        XMachine * agent, XModel * model) {
    unsigned int kk;
    int rc, errors = 0;

    /* For each input */
    for (kk = 0; kk < xfunction->getInputs()->size(); kk++) {
        /* Validate the communication */
        rc = validateAgentCommunication(xfunction->getInputs()->at(kk),
                agent, model);
        if (rc != 0) {
            printErr(std::string("       from input of message: ") +
                xfunction->getInputs()->at(kk)->getMessageName());
            errors += rc;
        }
    }

    /* For each output */
    for (kk = 0; kk < xfunction->getOutputs()->size(); kk++) {
        /* Validate the communication */
        rc = validateAgentCommunication(xfunction->getOutputs()->at(kk),
                agent, model);
        if (rc != 0) {
            printErr(std::string("       from output of message: ") +
                xfunction->getInputs()->at(kk)->getMessageName());
            errors += rc;
        }
    }
    return errors;
}

int XModelValidate::validateAgentFunction(XFunction * xfunction,
        XMachine * agent, XModel * model) {
    int errors = 0;

    /* Process function first */
    processAgentFunction(xfunction, agent->getVariables());

    /* Check name is valid */
    if (!name_is_allowed(xfunction->getName())) {
        printErr(std::string(
            "Error: Function name is not valid: ") +
            xfunction->getName());
        errors++;
    }

    /* Check current state name is valid */
    if (!name_is_allowed(xfunction->getCurrentState())) {
        printErr(std::string(
            "Error: Function current state name is not valid: ") +
            xfunction->getCurrentState());
        errors++;
    }

    /* Check next state name is valid */
    if (!name_is_allowed(xfunction->getNextState())) {
        printErr(std::string(
            "Error: Function next state name is not valid: ") +
            xfunction->getNextState());
        errors++;
    }

    /* If condition then process and validate */
    if (xfunction->getCondition() != 0)
        errors += validateAgentConditionOrFilter(xfunction->getCondition(),
                agent, 0, model);

    // Validate agent communication
    errors += validateAgentFunctionIOput(xfunction, agent, model);

    // Validate agent memory access
    // Todo Make sure variables are valid and only mentioned once

    return errors;
}

int XModelValidate::validateRandomString(XIOput * xioput) {
    if (xioput->getRandomString() == "true") {
        xioput->setRandom(true);
    } else if (xioput->getRandomString() == "false") {
        xioput->setRandom(false);
    } else {
        printErr(std::string(
        "Error: input random is not 'true' or 'false': ") +
            xioput->getRandomString());
        return 1;
    }

    return 0;
}

int XModelValidate::validateAgentCommunication(XIOput * xioput,
        XMachine * agent, XModel * model) {
    int errors = 0;
    XMessage * xmessage = model->getMessage(xioput->getMessageName());

    /* Check message type */
    if (xmessage == 0) { printErr(std::string(
        "Error: message name is not valid: ") +
            xioput->getMessageName());
        errors++;
    }

    /* If filter then process and validate */
    if (xioput->getFilter() != 0) errors += validateAgentConditionOrFilter(
        xioput->getFilter(), agent, xmessage, model);

    /* If sort then validate */
    if (xioput->isSort()) errors += validateSort(xioput, xmessage);

    /* If random then validate */
    if (xioput->isRandomSet()) errors += validateRandomString(xioput);

    /* Cannot be sorted and random at the same time */
    if (xioput->isSort() && xioput->isRandom()) { printErr(std::string(
            "Error: input cannot be sorted and random too: ") +
            xioput->getMessageName());
        errors++;
    }

    return errors;
}

int XModelValidate::validateAgentConditionOrFilter(XCondition * xcondition,
        XMachine * agent, XMessage * xmessage, XModel * model) {
    int rc, errors = 0;

    rc = xcondition->processSymbols();
    errors += rc;
    rc = xcondition->validate(agent, xmessage, model, xcondition);
    errors += rc;

    return errors;
}

int XModelValidate::validateSort(XIOput * xioput, XMessage * xmessage) {
    int errors = 0;

    /* Validate key as a message variable */
    /* Check message is valid */
    if (xmessage != 0) {
        if (!xmessage->validateVariableName(xioput->getSortKey())) {
            printErr(std::string(
                "Error: sort key is not a valid message variable: ") +
                xioput->getSortKey());
            errors++;
        }
    /* If message is not valid */
    } else {
        printErr(std::string(
    "Error: cannot validate sort key as the message type is invalid: ") +
           xioput->getSortKey());
       errors++;
    }
    /* Validate order as ascending or descending */
    if (xioput->getSortOrder() != "ascend" &&
            xioput->getSortOrder() != "descend") {
        printErr(std::string(
            "Error: sort order is not 'ascend' or 'descend': ") +
            xioput->getSortOrder());
        errors++;
    }

    return errors;
}

int XModelValidate::validateMessage(XMessage * xmessage, XModel * model) {
    int errors = 0;
    unsigned int ii;

    /* Check name is valid */
    if (!name_is_allowed(xmessage->getName())) {
        printErr(std::string("Error: Message name is not valid: ") +
            xmessage->getName());
        errors++;
    }

    /* Check for duplicate names */
    for (ii = 0; ii < model->getMessages()->size(); ii++)
        if (xmessage != model->getMessages()->at(ii) &&
            xmessage->getName() == model->getMessages()->at(ii)->getName()) {
            printErr(std::string("Error: Duplicate message name: ") +
                xmessage->getName());
            errors++;
        }

    errors += validateVariables(xmessage->getVariables(), model, false);

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
