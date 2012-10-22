/*!
 * \file src/model/xcondition.cpp
 * \author Simon Coakley
 * \date 2012
 * \copyright Copyright (c) 2012 STFC Rutherford Appleton Laboratory
 * \copyright Copyright (c) 2012 University of Sheffield
 * \copyright GNU Lesser General Public License
 * \brief XCondition: holds condition/filter information
 */
#include <boost/algorithm/string/predicate.hpp>
#include <boost/lexical_cast.hpp>
#include <cstdio>
#include <string>
#include <set>
#include "./xcondition.hpp"
#include "./xmachine.hpp"
#include "./xmessage.hpp"
#include "./xmodel.hpp"

void printErr(std::string message);

namespace flame { namespace model {

/*!
 * \brief Initialise all condition variables
 *
 * Initialise all condition variables.
 */
XCondition::XCondition() {
    timePhaseValue = 0;
    timePhaseIsVariable = false;
    isNot = false;
    isTime = false;
    isValues = false;
    isConditions = false;
    lhsCondition = 0;
    rhsCondition = 0;
    lhsIsAgentVariable = false;
    rhsIsAgentVariable = false;
    lhsIsMessageVariable = false;
    rhsIsMessageVariable = false;
    lhsIsValue = false;
    rhsIsValue = false;
    lhsIsCondition = false;
    rhsIsCondition = false;
    timePeriod = "";
    timePhaseVariable = "";
    timeDurationString = "";
    timeDuration = 0;
    foundTimeDuration = false;
    tempValue = "";
    lhs = "";
    op = "";
    rhs = "";
    lhsDouble = 0.0;
    rhsDouble = 0.0;
}

/*!
 * \brief Cleans up XCondtion
 *
 * Cleans up XCondtion by deleting any nexted conditions.
 */
XCondition::~XCondition() {
    /* Delete any nested conditions */
    if (lhsCondition != 0) delete lhsCondition;
    if (rhsCondition != 0) delete rhsCondition;
}

std::set<std::string> * XCondition::getReadOnlyVariables() {
    return &readOnlyVariables_;
}

void XCondition::printValues(std::string lhs, std::string op, std::string rhs,
        bool lhsIsAgentVariable, bool rhsIsAgentVariable,
        bool lhsIsMessageVariable, bool rhsIsMessageVariable,
        bool lhsIsValue, bool rhsIsValue, double lhsDouble, double rhsDouble) {
    /* Handle lhs */
    if (lhsIsAgentVariable) std::fprintf(stdout, "a.%s", lhs.c_str());
    else if (lhsIsMessageVariable) std::fprintf(stdout, "m.%s", lhs.c_str());
    else if (lhsIsValue) std::fprintf(stdout, "%f", lhsDouble);
    std::fprintf(stdout, " ");
    /* Handle operator */
    std::fprintf(stdout, "%s", op.c_str());
    std::fprintf(stdout, " ");
    /* Handle rhs */
    if (rhsIsAgentVariable) std::fprintf(stdout, "a.%s", rhs.c_str());
    else if (rhsIsMessageVariable) std::fprintf(stdout, "m.%s", rhs.c_str());
    else if (rhsIsValue) std::fprintf(stdout, "%f", rhsDouble);
}

void XCondition::printConditions(XCondition * lhsCondition,
        std::string op, XCondition * rhsCondition) {
    std::fprintf(stdout, "(");
    std::fprintf(stdout, "\n");
    lhsCondition->print();
    std::fprintf(stdout, "\t\t");
    std::fprintf(stdout, ") ");
    std::fprintf(stdout, "%s", op.c_str());
    std::fprintf(stdout, " (");
    std::fprintf(stdout, "\n");
    rhsCondition->print();
    std::fprintf(stdout, "\t\t");
    std::fprintf(stdout, ")");
}

void XCondition::printTime(std::string timePeriod,
        std::string timePhaseVariable, int timePhaseValue, int timeDuration,
        bool timePhaseIsVariable, bool foundTimeDuration) {
    std::fprintf(stdout, "time(");
    std::fprintf(stdout, "%s", timePeriod.c_str());
    std::fprintf(stdout, ", ");
    if (timePhaseIsVariable) std::fprintf(stdout, "a.%s",
            timePhaseVariable.c_str());
    else
        std::fprintf(stdout, "%d", timePhaseValue);
    if (foundTimeDuration) {
        std::fprintf(stdout, ", ");
        std::fprintf(stdout, "%d", timeDuration);
    }
    std::fprintf(stdout, ")");
}

/*!
 * \brief Print the condition to stdout
 *
 * Print the condition to standard out.
 */
void XCondition::print() {
    std::fprintf(stdout, "\t\t");
    if (isNot) {
        std::fprintf(stdout, "not(");
    }
    if (isValues) {
        printValues(lhs, op, rhs,
                lhsIsAgentVariable, rhsIsAgentVariable,
                lhsIsMessageVariable, rhsIsMessageVariable,
                lhsIsValue, rhsIsValue,
                lhsDouble, rhsDouble);
    }
    if (isConditions) {
        printConditions(lhsCondition, op, rhsCondition);
    }
    if (isTime) {
        printTime(timePeriod, timePhaseVariable,
                timePhaseValue, timeDuration, timePhaseIsVariable,
                foundTimeDuration);
    }
    if (isNot) {
        std::fprintf(stdout, ")");
    }
    std::fprintf(stdout, "\n");
}

int XCondition::processSymbolsTime() {
    int errors = 0;
    /* Handle agent phase variable */
    if (boost::starts_with(timePhaseVariable, "a.")) {
        timePhaseVariable.erase(0, 2);
        timePhaseIsVariable = true;
    } else {
        timePhaseIsVariable = false;
        /* Handle phase
           by trying to cast to int */
        try {
            timePhaseValue = boost::lexical_cast<int>(timePhaseVariable);
        } catch(const boost::bad_lexical_cast& E) {
            printErr(std::string(
                "Error: Cannot cast time phase to an integer: ") +
                timePhaseVariable);
            errors++;
        }
    }
    if (foundTimeDuration) {
        /* Handle duration number
           by trying to cast to int */
        try {
            timeDuration = boost::lexical_cast<int>(timeDurationString);
        } catch(const boost::bad_lexical_cast& E) {
            printErr(std::string(
                "Error: Cannot cast time duration to an integer: ") +
                timeDurationString);
            errors++;
        }
    }
    return errors;
}

int XCondition::processSymbolsValue(std::string * hs, bool * hsIsAgentVariable,
        bool * hsIsValue, bool * hsIsMessageVariable, double * hsDouble) {
    int errors = 0;
    /* Handle agent variable */
    if (boost::starts_with(*hs, "a.")) {
        *hsIsAgentVariable = true;
        *hsIsValue = false;
        hs->erase(0, 2);
    /* Handle message variable */
    } else if (boost::starts_with(*hs, "m.")) {
        *hsIsMessageVariable = true;
        *hsIsValue = false;
        hs->erase(0, 2);
    /* Handle number */
    } else {
        *hsIsValue = true;
        /* Try and cast to a double */
        try {
            *hsDouble = boost::lexical_cast<double>(*hs);
        } catch(const boost::bad_lexical_cast& E) {
            printErr(std::string(
        "Error: Condition/filter value not variable or number: ") +
                *hs);
            errors++;
        }
    }
    return errors;
}

int XCondition::processSymbolsValues() {
    int errors = 0;

    /* Process lhs value */
    errors += processSymbolsValue(&lhs, &lhsIsAgentVariable,
            &lhsIsValue, &lhsIsMessageVariable, &lhsDouble);
    /* Process rhs value */
    errors += processSymbolsValue(&rhs, &rhsIsAgentVariable,
                &rhsIsValue, &rhsIsMessageVariable, &rhsDouble);
    /* Process operator */
    if (op == "EQ") {
        op = "==";
    } else if (op == "NEQ") {
        op = "!=";
    } else if (op == "LEQ") {
        op = "<=";
    } else if (op == "GEQ") {
        op = ">=";
    } else if (op == "LT") {
        op = "<";
    } else if (op == "GT") {
        op = ">";
    } else {
        /* Handle unknown operator */
        printErr(std::string(
            "Error: Condition/filter op value not recognised: ") +
            op);
        errors++;
    }

    return errors;
}

int XCondition::processSymbolsConditions() {
    int errors = 0;
    int rc;

    rc = lhsCondition->processSymbols();
    errors += rc;
    rc = rhsCondition->processSymbols();
    errors += rc;

    if (op == "AND") {
        op = "&&";
    } else if (op == "OR") {
        op = "||";
    } else {
        printErr(std::string(
            "Error: Condition/filter op value not recognised: ") +
            op);
        errors++;
    }

    return errors;
}

/*!
 * \brief Processes symbols in conditions/filters
 * \return Number of errors
 * Handles agent/message variables and numbers for values and handles operators.
 */
int XCondition::processSymbols() {
    int errors = 0;

    if (isTime) {
        errors += processSymbolsTime();
    } else {
        /* Check lhs and rhs are both values or both conditions else error */
        if (lhsIsValue && rhsIsValue) {
            isValues = true;
            errors += processSymbolsValues();
        } else if (lhsIsCondition && rhsIsCondition) {
            isConditions = true;
            errors += processSymbolsConditions();
        } else {
            printErr(std::string(
        "Error: lhs and rhs are not both values or both nested conditions"));
            errors++;
        }
    }

    return errors;
}

int XCondition::validateTime(XMachine * agent, XModel * model,
        XCondition * rootCondition) {
    int errors = 0;
    unsigned int ii;
    /* Check time period is valid time unit */
    bool validPeriod = false;
    for (ii = 0; ii < model->getTimeUnits()->size(); ii++) {
        if (timePeriod == model->getTimeUnits()->at(ii)->getName())
            validPeriod = true;
    }
    /* Handle invalid time period */
    if (!validPeriod) {
        printErr(std::string(
            "Error: time period is not a valid time unit: ") +
            timePeriod);
        errors++;
    }
    /* If time phase is an agent variable then validate it */
    if (timePhaseIsVariable) {
        /* Handle invalid time phase variable */
        if (!agent->validateVariableName(timePhaseVariable)) {
            printErr(std::string(
    "Error: time phase variable is not a valid agent variable: ") +
                timePhaseVariable);
            errors++;
        } else {
            // If agent variable is valid then add to
            // read only variable list
            rootCondition->readOnlyVariables_.insert(
                agent->getVariable(timePhaseVariable)->getName());
        }
    }
    return errors;
}

int XCondition::validateValue(XMachine * agent, XMessage * xmessage,
        bool * hsIsAgentVariable, std::string * hs,
        bool * hsIsMessageVariable, XCondition * rootCondition) {
    /* Handle agent variable */
    if (*hsIsAgentVariable) {
        /* Try and validate */
        if (!agent->validateVariableName(*hs)) {
            printErr(std::string(
        "Error: value is not a valid agent variable: ") + *hs);
            return 1;
        } else {
            // If agent variable is valid then add to
            // read only variable list
rootCondition->readOnlyVariables_.insert(agent->getVariable(*hs)->getName());
        }
    /* Handle message variable */
    } else if (*hsIsMessageVariable) {
        /* If message type exists */
        if (xmessage != 0) {
            /* Try and validate */
            if (!xmessage->validateVariableName(*hs)) {
                printErr(std::string(
        "Error: value is not a valid message variable: ") + *hs);
                return 1;
            }
        } else {
            printErr(std::string(
        "Error: cannot validate value as the message type is invalid: ") +
                *hs);
            return 1;
        }
    }
    return 0;
}

int XCondition::validateValues(XMachine * agent, XMessage * xmessage,
        XCondition * rootCondition) {
    int errors = 0;
    /* If values validate any agent or message variables */
    errors += validateValue(agent, xmessage, &lhsIsAgentVariable,
            &lhs, &lhsIsMessageVariable, rootCondition);
    errors += validateValue(agent, xmessage, &rhsIsAgentVariable,
                &rhs, &rhsIsMessageVariable, rootCondition);

    return errors;
}

int XCondition::validate(XMachine * agent, XMessage * xmessage,
        XModel * model, XCondition * rootCondition) {
    int rc, errors = 0;

    if (isTime) {
        errors += validateTime(agent, model, rootCondition);
    } else if (isValues) {
        errors += validateValues(agent, xmessage, rootCondition);
    } else if (isConditions) {
        /* If nested conditions validate them */
        rc = lhsCondition->validate(agent, xmessage, model, rootCondition);
        errors += rc;
        rc = rhsCondition->validate(agent, xmessage, model, rootCondition);
        errors += rc;
    }

    return errors;
}

}}  // namespace flame::model
