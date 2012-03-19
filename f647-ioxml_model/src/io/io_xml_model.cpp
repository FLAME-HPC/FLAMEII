/*!
 * \file src/io/io_xml_model.cpp
 * \author Simon Coakley
 * \date 2012
 * \copyright Copyright (c) 2012 STFC Rutherford Appleton Laboratory
 * \copyright Copyright (c) 2012 University of Sheffield
 * \copyright GNU Lesser General Public License
 * \brief IOXMLModel: reading of model XML file
 */
#include "io_xml_model.hpp"
#include <boost/property_tree/xml_parser.hpp>
#include <boost/foreach.hpp>
#include <fstream>
#include <string>
#include <vector>
#include <cstdio>

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
                rc = readEnvironment(v);
                if (rc != 0) {
                    myfile.close();
                    return rc;
                }
            } else if (v.first == "agents") {
                readAgents(v, model);
            } else if (v.first == "messages") {
                //
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
        boost::property_tree::ptree::value_type const& root) {
    int rc; /* Return code */

    /* Loop through each child of environment */
    BOOST_FOREACH(boost::property_tree::ptree::value_type const& v,
            root.second ) {
        /* Handle each child */
        if (v.first == "file") {
            // std::cout << "'" << root.second.get<std::string>("file")
            // << "'\n";
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
        boost::property_tree::ptree::value_type const& root) {
    int rc; /* Return code */

    /* Loop through each child of environment */
    BOOST_FOREACH(boost::property_tree::ptree::value_type const& v,
            root.second ) {
        /* Handle each child */
        if (v.first == "constants") {
            // rc = readVariables(v);
            if (rc != 0) return rc;
        } else if (v.first == "timeUnits") {
            // rc = readModels(v);
            // if(rc != 0) return rc;
        } else if (v.first == "functionFiles") {
            rc = readFunctionFiles(v);
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
            rc = readTransitions(v);
            if (rc != 0) return rc;
        } else {
            rc = readUnknownElement(v);
            if (rc != 0) return rc;
        }
    }
    return 0;
}

int IOXMLModel::readInputs(
        boost::property_tree::ptree::value_type const& root) {
    int rc; /* Return code */

    /* Loop through each child of inputs */
    BOOST_FOREACH(boost::property_tree::ptree::value_type const& v,
            root.second ) {
        /* Handle each child */
        if (v.first == "input") {
            rc = readInput(v);
            if (rc != 0) return rc;
        } else {
            rc = readUnknownElement(v);
            if (rc != 0) return rc;
        }
    }
    return 0;
}

int IOXMLModel::readInput(
        boost::property_tree::ptree::value_type const& root) {
    int rc; /* Return code */

    /* Loop through each child of input */
    BOOST_FOREACH(boost::property_tree::ptree::value_type const& v,
            root.second ) {
        /* Handle each child */
        if (v.first == "messageName") {
            // std::cout << "'" << root.second.get<std::string>("messageName")
            //        << "'\n";
        } else if (v.first == "filter") {
            //
        } else if (v.first == "sort") {
            //
        } else if (v.first == "random") {
            if (root.second.get<std::string>("random") == "true") {}
                // std::cout << "random = true";
        } else {
            rc = readUnknownElement(v);
            if (rc != 0) return rc;
        }
    }
    return 0;
}

int IOXMLModel::readOutputs(
        boost::property_tree::ptree::value_type const& root) {
    int rc; /* Return code */

    /* Loop through each child of outputs */
    BOOST_FOREACH(boost::property_tree::ptree::value_type const& v,
            root.second ) {
        /* Handle each child */
        if (v.first == "output") {
            rc = readInput(v);
            if (rc != 0) return rc;
        } else {
            rc = readUnknownElement(v);
            if (rc != 0) return rc;
        }
    }
    return 0;
}

int IOXMLModel::readOutput(
        boost::property_tree::ptree::value_type const& root) {
    int rc; /* Return code */

    /* Loop through each child of output */
    BOOST_FOREACH(boost::property_tree::ptree::value_type const& v,
            root.second ) {
        /* Handle each child */
        if (v.first == "messageName") {
            // std::cout << "'" << root.second.get<std::string>("messageName")
            //        << "'\n";
        } else {
            rc = readUnknownElement(v);
            if (rc != 0) return rc;
        }
    }
    return 0;
}

int IOXMLModel::readTransition(
        boost::property_tree::ptree::value_type const& root) {
    int rc; /* Return code */

    /* Loop through each child of function */
    BOOST_FOREACH(boost::property_tree::ptree::value_type const& v,
            root.second ) {
        /* Handle each child */
        if (v.first == "name") {
            // std::cout << "'" << root.second.get<std::string>("name")
            // << "'\n";
        } else if (v.first == "description") {
            // std::cout << "'" << root.second.get<std::string>("description")
            //        << "'\n";
        } else if (v.first == "currentState") {
            // std::cout << "'" << root.second.get<std::string>("currentState")
            //        << "'\n";
        } else if (v.first == "nextState") {
            // std::cout << "'" << root.second.get<std::string>("nextState")
            //        << "'\n";
        } else if (v.first == "condition") {
            //
        } else if (v.first == "outputs") {
            rc = readOutputs(v);
            if (rc != 0) return rc;
        } else if (v.first == "inputs") {
            rc = readInputs(v);
            if (rc != 0) return rc;
        } else {
            rc = readUnknownElement(v);
            if (rc != 0) return rc;
        }
    }
    return 0;
}

int IOXMLModel::readTransitions(
        boost::property_tree::ptree::value_type const& root) {
    int rc; /* Return code */

    /* Loop through each child of functions */
    BOOST_FOREACH(boost::property_tree::ptree::value_type const& v,
            root.second ) {
        /* Handle each child */
        if (v.first == "function") {
            rc = readTransition(v);
            if (rc != 0) return rc;
        } else {
            rc = readUnknownElement(v);
            if (rc != 0) return rc;
        }
    }
    return 0;
}

}}  // namespace flame::io
