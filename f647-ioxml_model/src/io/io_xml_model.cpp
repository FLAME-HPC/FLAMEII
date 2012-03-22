/*!
 * \file src/io/io_xml_model.cpp
 * \author Simon Coakley
 * \date 2012
 * \copyright Copyright (c) 2012 STFC Rutherford Appleton Laboratory
 * \copyright Copyright (c) 2012 University of Sheffield
 * \copyright GNU Lesser General Public License
 * \brief IOXMLModel: reading of model XML file
 */
#include <boost/property_tree/xml_parser.hpp>
#include <boost/foreach.hpp>
#include <fstream>
#include <string>
#include <vector>
#include <cstdio>
#include "io_xml_model.hpp"
#include "./xadt.hpp"
#include "./xtimeunit.hpp"
#include "./xmessage.hpp"

namespace flame { namespace io {

int IOXMLModel::readXMLModel(std::string file_name, XModel * model) {
    int rc; /* Return code */
    /* Create stream and open file */
    std::ifstream myfile;
    myfile.open(file_name.c_str());

    /* If file opened successfully */
    if (myfile.is_open()) {
        /* Create property tree and populate, ignoring XML comments */
        boost::property_tree::ptree pt;
        try {
            read_xml(myfile, pt,
                    boost::property_tree::xml_parser::no_comments);
        } catch(const boost::property_tree::xml_parser_error& E) {
            /* Catch parser error */
            std::fprintf(stderr,
                    "Model file cannot be parsed: %s on line: %lu - %s\n",
                    file_name.c_str(), E.line(), E.message().c_str());
            myfile.close();
            return 2;
        }

        /* Catch error if no root called xmodel */
        try {
            pt.get_child("xmodel");
        } catch(std::runtime_error&) {
            std::fprintf(stderr,
                    "Model file does not have root called 'xmodel': %s\n",
                    file_name.c_str());
            myfile.close();
            return 3;
        }

        /* Loop through each child of xmodel */
        BOOST_FOREACH(boost::property_tree::ptree::value_type const& v,
                pt.get_child("xmodel") ) {
            /* Handle each child */
            if (v.first == "<xmlattr>") {
                /* Catch error if version is not 2 */
                if (v.second.get("version", 0) != 2) {
                    std::fprintf(stderr,
                            "Model file is not 'xmodel' version 2: %s\n",
                            file_name.c_str());
                    myfile.close();
                    return 4;
                }
            } else if (v.first == "name") {
                model->setName(pt.get<std::string>("xmodel.name"));
            } else if (v.first == "version") {
                // std::cout << "'" << pt.get<std::string>("xmodel.version")
                //        << "'\n";
            } else if (v.first == "author") {
                // std::cout << "'" << pt.get<std::string>("xmodel.author")
                //        << "'\n";
            } else if (v.first == "description") {
                // std::cout << "'" << pt.get<std::string>("xmodel.description")
                //        << "'\n";
            } else if (v.first == "models") {
                // rc = readModels(v);
                // if(rc != 0) return rc;
            } else if (v.first == "environment") {
                rc = readEnvironment(v, model);
                if (rc != 0) {
                    myfile.close();
                    return rc;
                }
            } else if (v.first == "agents") {
                rc = readAgents(v, model);
                if (rc != 0) {
                    myfile.close();
                    return rc;
                }
            } else if (v.first == "messages") {
                rc = readMessages(v, model);
                if (rc != 0) {
                    myfile.close();
                    return rc;
                }
            } else {
                rc = readUnknownElement(v);
                if (rc != 0) {
                    myfile.close();
                    return rc;
                }
            }
        }

        myfile.close();
        return 0;
    } else {
        /* Return error if the file was not successfully opened */
        std::fprintf(stderr,
                "Model file cannot be opened: %s\n", file_name.c_str());
        return 1;
    }
}

int IOXMLModel::readUnknownElement(
        boost::property_tree::ptree::value_type const& v) {
    std::fprintf(stderr,
            "Model file has unknown child '%s'\n", v.first.c_str());
    return 4;
}

int IOXMLModel::readFunctionFiles(
        boost::property_tree::ptree::value_type const& root,
        XModel * model) {
    int rc; /* Return code */

    /* Loop through each child of functionFiles */
    BOOST_FOREACH(boost::property_tree::ptree::value_type const& v,
            root.second ) {
        /* Handle each child */
        if (v.first == "file") {
            model->addFunctionFile(root.second.get<std::string>("file"));
        } else {
            rc = readUnknownElement(v);
            if (rc != 0) return rc;
        }
    }
    return 0;
}

int IOXMLModel::readDataTypes(
        boost::property_tree::ptree::value_type const& root,
        XModel * model) {
    int rc; /* Return code */

    /* Loop through each child of dataTypes */
    BOOST_FOREACH(boost::property_tree::ptree::value_type const& v,
            root.second ) {
        /* Handle each child */
        if (v.first == "dataType") {
            rc = readDataType(v, model);
            if (rc != 0) return rc;
        } else {
            rc = readUnknownElement(v);
            if (rc != 0) return rc;
        }
    }
    return 0;
}

int IOXMLModel::readDataType(
        boost::property_tree::ptree::value_type const& root,
        XModel * model) {
    int rc; /* Return code */
    XADT * xadt = model->addADT();

    /* Loop through each child of dataType */
    BOOST_FOREACH(boost::property_tree::ptree::value_type const& v,
            root.second ) {
        /* Handle each child */
        if (v.first == "name") {
            xadt->setName(root.second.get<std::string>("name"));
        } else if (v.first == "variables") {
            rc = readVariables(v, xadt->getVariables());
            if (rc != 0) return rc;
        } else {
            rc = readUnknownElement(v);
            if (rc != 0) return rc;
        }
    }
    return 0;
}

int IOXMLModel::readTimeUnits(
        boost::property_tree::ptree::value_type const& root,
        XModel * model) {
    int rc; /* Return code */

    /* Loop through each child of timeUnits */
    BOOST_FOREACH(boost::property_tree::ptree::value_type const& v,
            root.second ) {
        /* Handle each child */
        if (v.first == "timeUnit") {
            rc = readTimeUnit(v, model);
            if (rc != 0) return rc;
        } else {
            rc = readUnknownElement(v);
            if (rc != 0) return rc;
        }
    }
    return 0;
}

int IOXMLModel::readTimeUnit(
        boost::property_tree::ptree::value_type const& root,
        XModel * model) {
    int rc; /* Return code */
    XTimeUnit * xtimeunit = model->addTimeUnit();

    /* Loop through each child of timeUnit */
    BOOST_FOREACH(boost::property_tree::ptree::value_type const& v,
            root.second ) {
        /* Handle each child */
        if (v.first == "name") {
            xtimeunit->setName(root.second.get<std::string>("name"));
        } else if (v.first == "unit") {
            xtimeunit->setUnit(root.second.get<std::string>("unit"));
        } else if (v.first == "period") {
            xtimeunit->setPeriod(root.second.get<unsigned int>("period"));
        } else {
            rc = readUnknownElement(v);
            if (rc != 0) return rc;
        }
    }
    return 0;
}

int IOXMLModel::readVariables(
        boost::property_tree::ptree::value_type const& root,
        std::vector<XVariable*> * variables) {
    int rc; /* Return code */

    /* Loop through each child of variables */
    BOOST_FOREACH(boost::property_tree::ptree::value_type const& v,
            root.second ) {
        /* Handle each child */
        if (v.first == "variable") {
            rc = readVariable(v, variables);
            if (rc != 0) return rc;
        } else {
            rc = readUnknownElement(v);
            if (rc != 0) return rc;
        }
    }
    return 0;
}

int IOXMLModel::readVariable(
        boost::property_tree::ptree::value_type const& root,
        std::vector<XVariable*> * variables) {
    int rc; /* Return code */
    XVariable * xvariable = new XVariable;
    variables->push_back(xvariable);

    /* Loop through each child of variable */
    BOOST_FOREACH(boost::property_tree::ptree::value_type const& v,
            root.second ) {
        /* Handle each child */
        if (v.first == "type") {
            // std::cout << "'" << root.second.get<std::string>("type") << "' ";
            xvariable->setType(root.second.get<std::string>("type"));
        } else if (v.first == "name") {
            // std::cout << "'" << root.second.get<std::string>("name")
            // << "'\n";
            xvariable->setName(root.second.get<std::string>("name"));
        } else if (v.first == "description") {
            // std::cout << "'" << root.second.get<std::string>("description")
            // << "'\n";
        } else {
            rc = readUnknownElement(v);
            if (rc != 0) return rc;
        }
    }
    return 0;
}

int IOXMLModel::readEnvironment(
        boost::property_tree::ptree::value_type const& root,
        XModel * model) {
    int rc; /* Return code */

    /* Loop through each child of environment */
    BOOST_FOREACH(boost::property_tree::ptree::value_type const& v,
            root.second ) {
        /* Handle each child */
        if (v.first == "constants") {
            rc = readVariables(v, model->getConstants());
            if (rc != 0) return rc;
        } else if (v.first == "dataTypes") {
            rc = readDataTypes(v, model);
            if (rc != 0) return rc;
        } else if (v.first == "timeUnits") {
            rc = readTimeUnits(v, model);
            if (rc != 0) return rc;
        } else if (v.first == "functionFiles") {
            rc = readFunctionFiles(v, model);
            if (rc != 0) return rc;
        } else {
            rc = readUnknownElement(v);
            if (rc != 0) return rc;
        }
    }
    return 0;
}

int IOXMLModel::readAgents(
        boost::property_tree::ptree::value_type const& root, XModel * model) {
    int rc; /* Return code */

    /* Loop through each child of xagents */
    BOOST_FOREACH(boost::property_tree::ptree::value_type const& v,
            root.second ) {
        /* Handle each child */
        if (v.first == "xagent") {
            rc = readAgent(v, model);
            if (rc != 0) return rc;
        } else {
            rc = readUnknownElement(v);
            if (rc != 0) return rc;
        }
    }
    return 0;
}

int IOXMLModel::readAgent(boost::property_tree::ptree::value_type const& root,
        XModel * model) {
    int rc; /* Return code */
    XMachine * xm = model->addAgent();

    /* Loop through each child of xagent */
    BOOST_FOREACH(boost::property_tree::ptree::value_type const& v,
            root.second ) {
        /* Handle each child */
        if (v.first == "name") {
            xm->setName(root.second.get<std::string>("name"));
        } else if (v.first == "description") {
            // std::cout << "'" << root.second.get<std::string>("description")
            //        << "'\n";
        } else if (v.first == "memory") {
            rc = readVariables(v, xm->getVariables());
            if (rc != 0) return rc;
        } else if (v.first == "functions") {
            rc = readTransitions(v, xm);
            if (rc != 0) return rc;
        } else {
            rc = readUnknownElement(v);
            if (rc != 0) return rc;
        }
    }
    return 0;
}

int IOXMLModel::readInputs(
        boost::property_tree::ptree::value_type const& root,
        XFunction * xfunction) {
    int rc; /* Return code */

    /* Loop through each child of inputs */
    BOOST_FOREACH(boost::property_tree::ptree::value_type const& v,
            root.second ) {
        /* Handle each child */
        if (v.first == "input") {
            rc = readInput(v, xfunction);
            if (rc != 0) return rc;
        } else {
            rc = readUnknownElement(v);
            if (rc != 0) return rc;
        }
    }
    return 0;
}

int IOXMLModel::readInput(
        boost::property_tree::ptree::value_type const& root,
        XFunction * xfunction) {
    int rc; /* Return code */
    XIOput * input = xfunction->addInput();

    /* Loop through each child of input */
    BOOST_FOREACH(boost::property_tree::ptree::value_type const& v,
            root.second ) {
        /* Handle each child */
        if (v.first == "messageName") {
            input->setMessageName(root.second.get<std::string>("messageName"));
        } else if (v.first == "filter") {
            XCondition * xcondition = input->addFilter();
            rc = readCondition(v, xcondition);
            if (rc != 0) return rc;
        } else if (v.first == "sort") {
            rc = readSort(v, input);
            if (rc != 0) return rc;
        } else if (v.first == "random") {
            if (root.second.get<std::string>("random") == "true")
                input->setRandom(true);
        } else {
            rc = readUnknownElement(v);
            if (rc != 0) return rc;
        }
    }
    return 0;
}

int IOXMLModel::readOutputs(
        boost::property_tree::ptree::value_type const& root,
        XFunction * xfunction) {
    int rc; /* Return code */

    /* Loop through each child of outputs */
    BOOST_FOREACH(boost::property_tree::ptree::value_type const& v,
            root.second ) {
        /* Handle each child */
        if (v.first == "output") {
            rc = readOutput(v, xfunction);
            if (rc != 0) return rc;
        } else {
            rc = readUnknownElement(v);
            if (rc != 0) return rc;
        }
    }
    return 0;
}

int IOXMLModel::readOutput(
        boost::property_tree::ptree::value_type const& root,
        XFunction * xfunction) {
    int rc; /* Return code */
    XIOput * output = xfunction->addOutput();

    /* Loop through each child of output */
    BOOST_FOREACH(boost::property_tree::ptree::value_type const& v,
            root.second ) {
        /* Handle each child */
        if (v.first == "messageName") {
            output->setMessageName(root.second.get<std::string>("messageName"));
        } else {
            rc = readUnknownElement(v);
            if (rc != 0) return rc;
        }
    }
    return 0;
}

int IOXMLModel::readTransition(
        boost::property_tree::ptree::value_type const& root,
        XMachine * machine) {
    int rc; /* Return code */
    XFunction * xfunction = machine->addFunction();

    /* Loop through each child of transition */
    BOOST_FOREACH(boost::property_tree::ptree::value_type const& v,
            root.second ) {
        /* Handle each child */
        if (v.first == "name") {
            xfunction->setName(root.second.get<std::string>("name"));
        } else if (v.first == "description") {
        } else if (v.first == "currentState") {
            xfunction->setCurrentState(
                root.second.get<std::string>("currentState"));
        } else if (v.first == "nextState") {
            xfunction->setNextState(root.second.get<std::string>("nextState"));
        } else if (v.first == "condition") {
            XCondition * xcondition = xfunction->addCondition();
            rc = readCondition(v, xcondition);
            if (rc != 0) return rc;
        } else if (v.first == "outputs") {
            rc = readOutputs(v, xfunction);
            if (rc != 0) return rc;
        } else if (v.first == "inputs") {
            rc = readInputs(v, xfunction);
            if (rc != 0) return rc;
        } else {
            rc = readUnknownElement(v);
            if (rc != 0) return rc;
        }
    }
    return 0;
}

int IOXMLModel::readTransitions(
        boost::property_tree::ptree::value_type const& root,
        XMachine * machine) {
    int rc; /* Return code */

    /* Loop through each child of transitions */
    BOOST_FOREACH(boost::property_tree::ptree::value_type const& v,
            root.second ) {
        /* Handle each child */
        if (v.first == "function") {
            rc = readTransition(v, machine);
            if (rc != 0) return rc;
        } else {
            rc = readUnknownElement(v);
            if (rc != 0) return rc;
        }
    }
    return 0;
}

int IOXMLModel::readMessages(
        boost::property_tree::ptree::value_type const& root,
        XModel * model) {
    int rc; /* Return code */

    /* Loop through each child of messages */
    BOOST_FOREACH(boost::property_tree::ptree::value_type const& v,
            root.second ) {
        /* Handle each child */
        if (v.first == "message") {
            rc = readMessage(v, model);
            if (rc != 0) return rc;
        } else {
            rc = readUnknownElement(v);
            if (rc != 0) return rc;
        }
    }
    return 0;
}

int IOXMLModel::readMessage(
        boost::property_tree::ptree::value_type const& root,
        XModel * model) {
    int rc; /* Return code */
    XMessage * xmessage = model->addMessage();

    /* Loop through each child of message */
    BOOST_FOREACH(boost::property_tree::ptree::value_type const& v,
            root.second ) {
        /* Handle each child */
        if (v.first == "name") {
            xmessage->setName(root.second.get<std::string>("name"));
        } else if (v.first == "description") {
        } else if (v.first == "variables") {
            rc = readVariables(v, xmessage->getVariables());
            if (rc != 0) return rc;
        } else {
            rc = readUnknownElement(v);
            if (rc != 0) return rc;
        }
    }
    return 0;
}

int IOXMLModel::readSort(boost::property_tree::ptree::value_type const& root,
            XIOput * xioput) {
    int rc; /* Return code */
    xioput->setSort(true);

    /* Loop through each child of message */
    BOOST_FOREACH(boost::property_tree::ptree::value_type const& v,
            root.second ) {
        /* Handle each child */
        if (v.first == "key") {
            xioput->setSortKey(root.second.get<std::string>("key"));
        } else if (v.first == "order") {
            xioput->setSortOrder(root.second.get<std::string>("order"));
        } else {
            rc = readUnknownElement(v);
            if (rc != 0) return rc;
        }
    }
    return 0;
}

int IOXMLModel::readCondition(
        boost::property_tree::ptree::value_type const& root,
        XCondition * xcondition) {
    int rc; /* Return code */

    /* Loop through each child of message */
    BOOST_FOREACH(boost::property_tree::ptree::value_type const& v,
            root.second ) {
        /* Handle each child */
        if (v.first == "not") {
            xcondition->isNot = true;
            rc = readCondition(v, xcondition);
            if (rc != 0) return rc;
        } else if (v.first == "time") {
            xcondition->isTime = true;
            xcondition->isValues = false;
            xcondition->isConditions = false;
            BOOST_FOREACH(boost::property_tree::ptree::value_type const& v2,
                        v.second ) {
                if (v2.first == "period") {
                    xcondition->timePeriod =
                        v.second.get<std::string>("period");
                } else if (v2.first == "phase") {
                    xcondition->timePhaseVariable =
                        v.second.get<std::string>("phase");
                } else if (v2.first == "duration") {
                    xcondition->foundTimeDuration = true;
                    xcondition->timeDurationString =
                        v.second.get<std::string>("duration");
                } else {
                    rc = readUnknownElement(v2);
                    if (rc != 0) return rc;
                }
            }
        } else if (v.first == "lhs") {
            /* Set up and read lhs */
            xcondition->lhsCondition = new XCondition;
            xcondition->tempValue = "";
            rc = readCondition(v, xcondition->lhsCondition);
            if (rc != 0) return rc;
            /* Handle if lhs is a value or a condition */
            if (xcondition->lhsCondition->tempValue != "") {
                /* lhs is a value */
                xcondition->lhs = xcondition->lhsCondition->tempValue;
                xcondition->lhsIsValue = true;
                delete xcondition->lhsCondition;
                xcondition->lhsCondition = 0;
            } else {
                /* lhs is a nested condition */
                xcondition->lhsIsCondition = true;
            }
        } else if (v.first == "op") {
            xcondition->op = root.second.get<std::string>("op");
        } else if (v.first == "rhs") {
            /* Set up and read rhs */
            xcondition->rhsCondition = new XCondition;
            xcondition->tempValue = "";
            rc = readCondition(v, xcondition->rhsCondition);
            if (rc != 0) return rc;
            /* Handle if rhs is a value or a condition */
            if (xcondition->rhsCondition->tempValue != "") {
                /* rhs is a value */
                xcondition->rhs = xcondition->rhsCondition->tempValue;
                xcondition->rhsIsValue = true;
                delete xcondition->rhsCondition;
                xcondition->rhsCondition = 0;
            } else {
                /* rhs is a nested condition */
                xcondition->rhsIsCondition = true;
            }
        } else if (v.first == "value") {
            xcondition->tempValue = root.second.get<std::string>("value");
        } else {
            rc = readUnknownElement(v);
            if (rc != 0) return rc;
        }
    }
    return 0;
}

}}  // namespace flame::io
