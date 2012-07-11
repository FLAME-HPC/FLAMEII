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
#include "./xcondition.hpp"
#include "./xmachine.hpp"
#include "./xmessage.hpp"
#include "./xmodel.hpp"

namespace flame { namespace model {

/*!
 * \brief Initialise all condition variables
 *
 * Initialise all condition variables.
 */
XCondition::XCondition() {
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

XCondition::~XCondition() {
    /* Delete any nested conditions */
    if (lhsCondition != 0) delete lhsCondition;
    if (rhsCondition != 0) delete rhsCondition;
}

void printValues(std::string lhs, std::string op, std::string rhs,
        bool lhsIsAgentVariable, bool rhsIsAgentVariable,
        bool lhsIsMessageVariable, bool rhsIsMessageVariable,
        bool lhsIsValue, bool rhsIsValue,
        double lhsDouble, double rhsDouble) {
    if (lhsIsAgentVariable) {
        std::fprintf(stdout, "a.%s", lhs.c_str());
    } else if (lhsIsMessageVariable) {
        std::fprintf(stdout, "m.%s", lhs.c_str());
    } else if (lhsIsValue) {
        std::fprintf(stdout, "%f", lhsDouble);
    }
    std::fprintf(stdout, " ");
    std::fprintf(stdout, "%s", op.c_str());
    std::fprintf(stdout, " ");
    if (rhsIsAgentVariable) {
        std::fprintf(stdout, "a.%s", rhs.c_str());
    } else if (rhsIsMessageVariable) {
        std::fprintf(stdout, "m.%s", rhs.c_str());
    } else if (rhsIsValue) {
        std::fprintf(stdout, "%f", rhsDouble);
    }
}

void printConditions(XCondition * lhsCondition,
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

void printTime(std::string timePeriod, std::string timePhaseVariable,
        int timePhaseValue, int timeDuration, bool timePhaseIsVariable,
        bool foundTimeDuration) {
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
            std::fprintf(stderr,
                "Error: Cannot cast time phase to an integer: %s\n",
                timePhaseVariable.c_str());
            errors++;
        }
    }
    if (foundTimeDuration) {
        /* Handle duration number
           by trying to cast to int */
        try {
            timeDuration = boost::lexical_cast<int>(timeDurationString);
        } catch(const boost::bad_lexical_cast& E) {
            std::fprintf(stderr,
                "Error: Cannot cast time duration to an integer: %s\n",
                timeDurationString.c_str());
            errors++;
        }
    }
    return errors;
}

int processSymbolsValue(std::string * hs, bool * hsIsAgentVariable,
        bool * hsIsValue, bool * hsIsMessageVariable, double * hsDouble) {
    int errors = 0;
    if (boost::starts_with(*hs, "a.")) {
        *hsIsAgentVariable = true;
        *hsIsValue = false;
        hs->erase(0, 2);
    } else if (boost::starts_with(*hs, "m.")) {
        *hsIsMessageVariable = true;
        *hsIsValue = false;
        hs->erase(0, 2);
    } else {
        *hsIsValue = true;
        try {
            *hsDouble = boost::lexical_cast<double>(*hs);
        } catch(const boost::bad_lexical_cast& E) {
            std::fprintf(stderr,
        "Error: Condition/filter value not variable or number: %s\n",
                hs->c_str());
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
        std::fprintf(stderr,
            "Error: Condition/filter op value not recognised: %s\n",
            op.c_str());
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
        std::fprintf(stderr,
            "Error: Condition/filter op value not recognised: %s\n",
            op.c_str());
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
            std::fprintf(stderr, "Error: lhs and rhs are not both \
values or both nested conditions\n");
            errors++;
        }
    }

    return errors;
}

int XCondition::validateTime(XMachine * agent, XModel * model) {
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
        std::fprintf(stderr,
            "Error: time period is not a valid time unit: '%s',\n",
            timePeriod.c_str());
        errors++;
    }
    /* If time phase is an agent variable then validate it */
    if (timePhaseIsVariable) {
        /* Handle invalid time phase variable */
        if (!agent->validateVariableName(timePhaseVariable)) {
            std::fprintf(stderr,
    "Error: time phase variable is not a valid agent variable: '%s',\n",
                timePhaseVariable.c_str());
            errors++;
        }
    }
    return errors;
}

int validateValue(XMachine * agent, XMessage * xmessage, bool * hsIsAgentVariable,
        std::string * hs, bool * hsIsMessageVariable) {
    int errors = 0;
    if (*hsIsAgentVariable) {
        if (!agent->validateVariableName(*hs)) {
            std::fprintf(stderr,
                "Error: value is not a valid agent variable: '%s',\n",
                hs->c_str());
            errors++;
        }
    } else if (*hsIsMessageVariable) {
        if (xmessage != 0) {
            if (!xmessage->validateVariableName(*hs)) {
                std::fprintf(stderr,
                    "Error: value is not a valid message variable: '%s',\n",
                    hs->c_str());
                errors++;
            }
        } else {
            std::fprintf(stderr,
                "Error: cannot validate value as the \
                message type is invalid: '%s',\n",
                hs->c_str());
            errors++;
        }
    }
    return errors;
}

int XCondition::validateValues(XMachine * agent, XMessage * xmessage) {
    int errors = 0;
    /* If values validate any agent or message variables */
    errors += validateValue(agent, xmessage, &lhsIsAgentVariable,
            &lhs, &lhsIsMessageVariable);
    errors += validateValue(agent, xmessage, &rhsIsAgentVariable,
                &rhs, &rhsIsMessageVariable);

    return errors;
}

int XCondition::validate(XMachine * agent, XMessage * xmessage,
        XModel * model) {
    int rc, errors = 0;

    if (isTime) {
        errors += validateTime(agent, model);
    } else if (isValues) {
        errors += validateValues(agent, xmessage);
    } else if (isConditions) {
        /* If nested conditions validate them */
        rc = lhsCondition->validate(agent, xmessage, model);
        errors += rc;
        rc = rhsCondition->validate(agent, xmessage, model);
        errors += rc;
    }

    return errors;
}

}}  // namespace flame::model
