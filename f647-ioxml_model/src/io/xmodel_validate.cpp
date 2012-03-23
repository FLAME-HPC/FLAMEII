/*!
 * \file src/io/xmodel_check.cpp
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
#include "./xmodel.hpp"

namespace flame { namespace io {

int processVariables(std::vector<XVariable*> * variables_,
        XModel * model);
int validateVariables(std::vector<XVariable*> * variables_,
        XModel * model, bool allowDyamicArrays);
int validateAgent(XMachine * agent, XModel * model);
int validateAgentFunction(XFunction * xfunction,
        XMachine * agent, XModel * model);
int validateAgentCommunication(XIOput * xioput, XMachine * agent,
        XModel * model);
int validateAgentConditionOrFilter(XCondition * xcondition, XMachine * agent,
        XMessage * xmessage, XModel * model);
int validateSort(XIOput * xioput, XMessage * xmessage);
int validateMessage(XMessage * xmessage, XModel * model);

int XModel::validate() {
    int rc, errors = 0;
    unsigned int ii;


    /* MODEL WIDE */
    rc = validateVariables(&constants_, this, false);
    if (rc != 0) {
        std::fprintf(stderr,
            "from environment constants.\n");
        errors += rc;
    }

    /* Unique agent names */

    /* Unique message names */

    /* PER AGENT */
    for (ii = 0; ii < agents_.size(); ii++) {
        rc = validateAgent(agents_.at(ii), this);
        if (rc != 0) {
            std::fprintf(stderr,
                "from agent: '%s'.\n",
                agents_.at(ii)->getName().c_str());
            errors += rc;
        }
    }

    /* PER MESSAGE */
    for (ii = 0; ii < messages_.size(); ii++) {
        rc = validateMessage(messages_.at(ii), this);
        if (rc != 0) {
            std::fprintf(stderr,
                "from message: '%s'.\n",
                messages_.at(ii)->getName().c_str());
            errors += rc;
        }
    }

    /* FROM XPARSER TRUNK */
    /* Check to see if conditions contain agent variables */
    /* Check to see if conditions contain message variables */
    /* Handle variable type, _array etc */
    /* Handle variable name, [ ] etc */
    /* Check if datatype has dynamic arrays etc */
    /* Check messages for dynamic arrays */
    /* Check if condition variable is valid agent/message variable */
    /* Check if condition time period is valid */
    /* Check condition lhs and rhs are valid */
    /* Check condition op is valid */
    /* Check condition op IN has valid variables */
    /* Check for no agents */
    /* Check for duplicate messages names */
    /* Check agent memory variables for being a model data type and update
     * variable attributes */
        /* Remove agents with no memory variables */
        /* Error if variables type is not a C type or a data type or an
         * array of these */
        /* Error if a variable name is defined twice in same agent */
    /* Check condition/filter rules */
        /* Check if message type exists for input/output */
        /* Check sort variable and sort order */
        /* Variable must be a valid message variable */
        /* Sort order must be either ascend or descend */

    /* FROM XML SCHEMA V2 */
    /* false string is true or false */
    /* * sort order string is ascend or descend */
    /* c source file name is .c */
    /* xml file name is .xml or .XML */
    /* * op string is EQ NEQ LEQ GEQ LT GT IN AND OR */
    /* names of agents must be unique */
    /* names of environment constants must be unique */
    /* names of custom time units must be unique */
    /* names of custom data types must be unique */
    /* each nested model should only be referenced once */
    /* names of messages must be unique */
    /* * input and output messageNames must refer to a valid message name */

    /* check all names are valid C names? azAZ09_- */

    /* Return number of errors */
    if (errors > 0) {
        std::fprintf(stderr,
            "%d errors found.\n", errors);
    }
    return errors;
}

int validateAgent(XMachine * agent, XModel * model) {
    int rc, errors = 0;
    unsigned int ii;

    rc = validateVariables(agent->getVariables(), model, true);
    errors += rc;

    for (ii = 0; ii < agent->getFunctions()->size(); ii++) {
        rc = validateAgentFunction(agent->getFunctions()->at(ii),
                agent, model);
        if (rc != 0) {
            std::fprintf(stderr,
                "from function: '%s',\n",
                agent->getFunctions()->at(ii)->getName().c_str());
            errors += rc;
        }
    }

    return errors;
}

int processVariables(std::vector<XVariable*> * variables_,
        XModel * model) {
    int errors = 0;

    /* Handle dynamic arrays in variable type */

    /* Handle static arrays in variable name */

    return errors;
}

int validateVariables(std::vector<XVariable*> * variables,
        XModel * model, bool allowDyamicArrays) {
    int errors = 0;
    unsigned int ii, jj;
    bool foundValidDataType;

    /* Process variables first */
    processVariables(variables, model);

    /* Check for duplicate names */
    for (ii = 0; ii < variables->size(); ii++) {
        for (jj = 0; jj < variables->size(); jj++) {
            if (variables->at(ii) != variables->at(jj) &&
                variables->at(ii)->getName() ==
                variables->at(jj)->getName()) {
                std::fprintf(stderr,
                    "Error: Duplicate variable name: '%s',\n",
                    variables->at(ii)->getName().c_str());
                errors++;
            }
        }
    }
    /* Check for valid types */
    for (ii = 0; ii < variables->size(); ii++) {
        foundValidDataType = false;
        /* Check single data types */
        for (jj = 0; jj < model->getAllowedDataTypes()->size(); jj++) {
            if (variables->at(ii)->getType() ==
                    model->getAllowedDataTypes()->at(jj))
                foundValidDataType = true;
        }
        /* Check static array */

        /* Check dynamic array */

        if (!foundValidDataType) {
            std::fprintf(stderr,
                "Error: Data type: '%s' not valid for variable name: '%s',\n",
                variables->at(ii)->getType().c_str(),
                variables->at(ii)->getName().c_str());
            errors++;
        }
    }

    return errors;
}

int validateAgentFunction(XFunction * xfunction, XMachine * agent,
        XModel * model) {
    int rc, errors = 0;
    unsigned int kk;

    /* If condition then process and validate */
    if (xfunction->getCondition() != 0) {
        rc = validateAgentConditionOrFilter(xfunction->getCondition(),
                agent, 0, model);
        errors += rc;
    }

    /* For each input */
    for (kk = 0; kk < xfunction->getInputs()->size(); kk++) {
        rc = validateAgentCommunication(xfunction->getInputs()->at(kk),
                agent, model);
        if (rc != 0) {
            std::fprintf(stderr,
                "from input of message: %s,\n",
                xfunction->getInputs()->at(kk)->getMessageName().c_str());
            errors += rc;
        }
    }

    /* For each output */
    for (kk = 0; kk < xfunction->getOutputs()->size(); kk++) {
        rc = validateAgentCommunication(xfunction->getOutputs()->at(kk),
                agent, model);
        if (rc != 0) {
            std::fprintf(stderr,
                "from output of message: %s,\n",
                xfunction->getInputs()->at(kk)->getMessageName().c_str());
            errors += rc;
        }
    }

    return errors;
}

int validateAgentCommunication(XIOput * xioput, XMachine * agent,
        XModel * model) {
    int rc, errors = 0;
    XMessage * xmessage;

    xmessage = model->getMessage(xioput->getMessageName());

    /* Check message type */
    if (xmessage == 0) {
        std::fprintf(stderr, "Error: message name is not valid: '%s',\n",
                xioput->getMessageName().c_str());
        errors++;
    }

    /* If filter then process and validate */
    if (xioput->getFilter() != 0) {
        rc = validateAgentConditionOrFilter(xioput->getFilter(), agent,
                model->getMessage(xioput->getMessageName()), model);
        errors += rc;
    }
    /* If sort then validate */
    if (xioput->getSort()) {
        validateSort(xioput, xmessage);
    }

    return errors;
}

int validateAgentConditionOrFilter(XCondition * xcondition, XMachine * agent,
        XMessage * xmessage, XModel * model) {
    int rc, errors = 0;

    rc = xcondition->processSymbols();
    errors += rc;
    rc = xcondition->validate(agent, xmessage);
    errors += rc;

    return errors;
}

int validateSort(XIOput * xioput, XMessage * xmessage) {
    int errors = 0;

    /* Validate key as a message variable */
    if (xmessage != 0) {
        if (!xmessage->validateVariableName(xioput->getSortKey())) {
            std::fprintf(stderr,
                "Error: sort key is not a valid message variable: '%s',\n",
                xioput->getSortKey().c_str());
            errors++;
        }
    } else {
       std::fprintf(stderr,
    "Error: cannot validate sort key as the message type is invalid: '%s',\n",
           xioput->getSortKey().c_str());
       errors++;
    }
    /* Validate order as ascending or descending */
    if (xioput->getSortOrder() != "ascend" &&
            xioput->getSortOrder() != "descend") {
        std::fprintf(stderr,
            "Error: sort order is not 'ascend' or 'descend': '%s',\n",
            xioput->getSortOrder().c_str());
        errors++;
    }

    return errors;
}

int validateMessage(XMessage * xmessage, XModel * model) {
    int rc, errors = 0;
    // unsigned int ii;

    rc = validateVariables(xmessage->getVariables(), model, false);
    errors += rc;

    return errors;
}

}}  // namespace flame::io
