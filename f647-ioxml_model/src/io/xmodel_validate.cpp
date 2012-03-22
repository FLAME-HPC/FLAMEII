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

int validateAgent(XMachine * agent, XModel * model);
int validateVariables(std::vector<XVariable*> * variables_);
int validateAgentFunction(XFunction * xfunction,
        XMachine * agent, XModel * model);
int validateAgentCommunication(XIOput * xioput, XMachine * agent,
        XModel * model);
int validateAgentConditionOrFilter(XCondition * xcondition, XMachine * agent,
        XMessage * xmessage, XModel * model);
int validateSort(XIOput * input, XMessage * xmessage);

int XModel::validate() {
    int rc;
    unsigned int ii;


    /* MODEL WIDE */
    validateVariables(&constants_);

    /* Unique agent names */

    /* Unique message names */

    /* PER AGENT */
    for (ii = 0; ii < agents_.size(); ii++) {
        rc = validateAgent(agents_.at(ii), this);
        if (rc != 0) {
            std::fprintf(stderr,
                "from agent: %s\n",
                agents_.at(ii)->getName().c_str());
            return rc;
        }
    }

    /* PER MESSAGE */

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
    /* sort order string is ascend or descend */
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

    /* Model is valid */
    return 0;
}

int validateAgent(XMachine * agent, XModel * model) {
    int rc;
    unsigned int ii;

    validateVariables(agent->getVariables());

    for (ii = 0; ii < agent->getFunctions()->size(); ii++) {
        rc = validateAgentFunction(agent->getFunctions()->at(ii),
                agent, model);
        if (rc != 0) {
            std::fprintf(stderr,
                "from function: %s\n",
                agent->getFunctions()->at(ii)->getName().c_str());
            return rc;
        }
    }
    return 0;
}

int validateVariables(std::vector<XVariable*> * variables_) {
    /* Check for duplicate names */

    /* Check for valid types */

    return 0;
}

int validateAgentFunction(XFunction * xfunction, XMachine * agent,
        XModel * model) {
    int rc;
    unsigned int kk;

    /* If condition then process and validate */
    if (xfunction->getCondition() != 0) {
        rc = validateAgentConditionOrFilter(xfunction->getCondition(),
                agent, 0, model);
        if (rc != 0) return rc;
    }

    /* For each input */
    for (kk = 0; kk < xfunction->getInputs()->size(); kk++) {
        rc = validateAgentCommunication(xfunction->getInputs()->at(kk),
                agent, model);
        if (rc != 0) {
            std::fprintf(stderr,
                "from input of message: %s\n",
                xfunction->getInputs()->at(kk)->getMessageName().c_str());
            return rc;
        }
    }

    /* For each output */
    for (kk = 0; kk < xfunction->getOutputs()->size(); kk++) {
        rc = validateAgentCommunication(xfunction->getOutputs()->at(kk),
                agent, model);
        if (rc != 0) {
            std::fprintf(stderr,
                "from output of message: %s\n",
                xfunction->getInputs()->at(kk)->getMessageName().c_str());
            return rc;
        }
    }

    return 0;
}

int validateAgentCommunication(XIOput * xioput, XMachine * agent,
        XModel * model) {
    int rc;
    XMessage * xmessage;

    xmessage = model->getMessage(xioput->getMessageName());

    /* Check message type */
    if (xmessage == 0) {
        std::fprintf(stderr, "message name is not valid\n");
        return 1;
    }

    /* If filter then process and validate */
    if (xioput->getFilter() != 0) {
        rc = validateAgentConditionOrFilter(xioput->getFilter(), agent,
                model->getMessage(xioput->getMessageName()), model);
        if (rc != 0) return rc;
    }
    /* If sort then validate */
    if (xioput->getSort()) {
        /* Validate key as a message variable */
        if (!xmessage->validateVariableName(xioput->getSortKey())) {
            std::fprintf(stderr,
                "sort key is not a valid message variable: %s\n",
                xioput->getSortKey().c_str());
            return 1;
        }
        /* Validate order as ascending or descending */
        if (xioput->getSortOrder() != "ascend" &&
                xioput->getSortOrder() != "descend") {
            std::fprintf(stderr,
                "sort order is not 'ascend' or 'descend': %s\n",
                xioput->getSortOrder().c_str());
            return 1;
        }
    }

    return 0;
}

int validateAgentConditionOrFilter(XCondition * xcondition, XMachine * agent,
        XMessage * xmessage, XModel * model) {
    int rc;

    rc = xcondition->processSymbols();
    if (rc != 0) return rc;
    rc = xcondition->validate(agent, xmessage);
    if (rc != 0) return rc;

    return 0;
}

int validateSort(XIOput * input, XMessage * xmessage) {
    return 0;
}

}}  // namespace flame::io
