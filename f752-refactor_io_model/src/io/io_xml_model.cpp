/*!
 * \file src/io/io_xml_model.cpp
 * \author Simon Coakley
 * \date 2012
 * \copyright Copyright (c) 2012 STFC Rutherford Appleton Laboratory
 * \copyright Copyright (c) 2012 University of Sheffield
 * \copyright GNU Lesser General Public License
 * \brief IOXMLModel: reading of model XML file
 */
#include <boost/algorithm/string/predicate.hpp>
#include <boost/filesystem.hpp>
#include <string>
#include <vector>
#include <cstdio>
#include "io_xml_model.hpp"

namespace model = flame::model;

namespace flame { namespace io { namespace xml {

void IOXMLModel::print_element_names(xmlNode * a_node) {
    xmlNode *cur_node = NULL;

    for (cur_node = a_node; cur_node; cur_node = cur_node->next) {
        if (cur_node->type == XML_ELEMENT_NODE) {
            printf("node type: Element, name: %s\n", cur_node->name);
        }

        print_element_names(cur_node->children);
    }
}

std::string IOXMLModel::getElementName(xmlNode * node) {
    std::string s = reinterpret_cast<const char*>(node->name);
    return s;
}

std::string IOXMLModel::getElementValue(xmlNode * node) {
    std::string s = reinterpret_cast<const char*>(node->children->content);
    return s;
}

int IOXMLModel::validateXMLModelRootElement(
        xmlNode *root_element, std::string file_name) {
    /* Catch error if no root called xmodel */
    if (getElementName(root_element) != "xmodel") {
        std::fprintf(stderr,
            "Error: Model file does not have root called 'xmodel': %s\n",
                file_name.c_str());
        return 3;
    }

    /* Catch error if version is not 2 */
    xmlChar * version_ptr = xmlGetProp(root_element, (const xmlChar*)"version");
    std::string version = reinterpret_cast<const char*>(version_ptr);
    xmlFree(version_ptr);
    if (version != "2") {
        std::fprintf(stderr,
    "Error: Model file is not 'xmodel' version 2: %s\n", file_name.c_str());
        return 4;
    }

    return 0;
}

int IOXMLModel::readXMLModel(std::string file_name, model::XModel * model) {
    /* Used for return codes */
    int rc;
    /* Directory of the model file */
    std::string directory;
    xmlDoc *doc = NULL;
    xmlNode *root_element = NULL;

    /* Find file directory to help open any submodels */
    boost::filesystem3::path filePath(file_name);
    directory = filePath.parent_path().string();
    directory.append("/");

    /* Print out diagnostics */
    fprintf(stdout, "Reading '%s'\n", file_name.c_str());

    /* Save absolute path to check the file is not read again */
    model->setPath(boost::filesystem3::absolute(filePath).string());

    /* Parse the file and get the DOM */
    doc = xmlReadFile(file_name.c_str(), NULL, 0);

    /* Check if file opened successfully */
    if (doc == NULL) {
        /* Return error if the file was not successfully parsed */
        std::fprintf(stderr, "Error: Model file cannot be opened/parsed: %s\n",
                file_name.c_str());
        return 1;
    }

    /*Get the root element node */
    root_element = xmlDocGetRootElement(doc);

    rc = validateXMLModelRootElement(root_element, file_name);
    if (rc == 0) rc = readModelElements(root_element, model, directory);

    xmlFreeDoc(doc);
    return rc;
}

int IOXMLModel::readModelElements(xmlNode *root_element, model::XModel * model,
        std::string directory) {
    int rc = 0;
    /* Loop through each child of xmodel */
    xmlNode *cur_node = NULL;
    for (cur_node = root_element->children;
            cur_node; cur_node = cur_node->next) {
        /* If node is an XML element */
        if (cur_node->type == XML_ELEMENT_NODE) {
            /* Handle each child */
            std::string name = getElementName(cur_node);
            if (name == "name")
                model->setName(getElementValue(cur_node));
            /* Version/Author/Description not read */
            else if (name == "version") {}
            else if (name == "author") {}
            else if (name == "description") {}
            else if (name == "models")
                rc = readIncludedModels(cur_node, directory, model);
            else if (name == "environment")
                rc = readEnvironment(cur_node, model);
            else if (name == "agents")
                rc = readAgents(cur_node, model);
            else if (name == "messages")
                rc = readMessages(cur_node, model);
            else
                rc = readUnknownElement(cur_node);
            /* Return if any errors */
            if (rc != 0) return rc;
        }
    }
    return 0;
}

int IOXMLModel::readUnknownElement(xmlNode * node) {
    std::fprintf(stderr,
            "Warning: Model file has unknown element '%s'\n", node->name);

    /* Return zero to carry on as normal */
    return 0;
}

int IOXMLModel::readIncludedModels(xmlNode * node,
        std::string directory, model::XModel * model) {
    int rc; /* Return code */
    xmlNode *cur_node = NULL;

    /* Loop through each child of models */
    for (cur_node = node->children;
                cur_node; cur_node = cur_node->next) {
        /* If node is an XML element */
        if (cur_node->type == XML_ELEMENT_NODE) {
            std::string name = getElementName(cur_node);
            /* Handle each child */
            if (name == "model") {
                rc = readIncludedModel(cur_node, directory, model);
                if (rc != 0) return rc;
            } else {
                rc = readUnknownElement(cur_node);
                if (rc != 0) return rc;
            }
        }
    }
    return 0;
}

int IOXMLModel::readIncludedModelValidate(std::string directory,
        std::string fileName, model::XModel * model, bool enable) {
    int rc;
    /* If included model is enabled */
    if (enable) {
        /* Check file name ends in '.xml' or '.XML' */
        if (!boost::algorithm::ends_with(fileName, ".xml") &&
                !boost::algorithm::ends_with(fileName, ".XML")) {
            std::fprintf(stderr, "Error: %s '%s'\n",
                "Included model file does not end in '.xml':",
                fileName.c_str());
            return 6;
        }

        /* Append file name to current model file directory */
        fileName = directory.append(fileName);

        /* Check sub model is not a duplicate */
        if (!model->addIncludedModel(
            boost::filesystem3::absolute(fileName).string())) {
            std::fprintf(stderr,
                "Error: Included model is a duplicate: '%s'\n",
                fileName.c_str());
            return 7;
        }

        /* Read model file... */
        rc = readXMLModel(fileName, model);
        if (rc != 0) {
            std::fprintf(stderr,
                "       from included model '%s'\n", fileName.c_str());
            return 8;
        }
    }

    return 0;
}

int IOXMLModel::readIncludedModel(xmlNode * node,
        std::string directory, model::XModel * model) {
    int rc = 0; /* Return code */
    xmlNode *cur_node = NULL;
    std::string fileName;
    bool enable;

    /* Loop through each child of dataType */
    for (cur_node = node->children; cur_node; cur_node = cur_node->next) {
        /* If node is an XML element */
        if (cur_node->type == XML_ELEMENT_NODE) {
            std::string name = getElementName(cur_node);
            /* Handle each child */
            if (name == "file") fileName = getElementValue(cur_node);
            else if (name == "enabled") {
                std::string enabledString = getElementValue(cur_node);
                if (enabledString == "true") { enable = true;
                } else if (enabledString == "false") { enable = false;
                } else {
std::fprintf(stderr, "Error: Included model has invalid enabled value '%s'\n",
                        enabledString.c_str());
                    return 5;
                }
            } else {
                rc = readUnknownElement(cur_node);
                if (rc != 0) return rc;
            }
        }
    }

    /* Handle enabled models */
    rc = readIncludedModelValidate(directory, fileName, model, enable);

    return rc;
}


int IOXMLModel::readFunctionFiles(xmlNode * node,
        model::XModel * model) {
    int rc; /* Return code */
    xmlNode *cur_node = NULL;

    /* Loop through each child of functionFiles */
    for (cur_node = node->children;
            cur_node; cur_node = cur_node->next) {
        /* If node is an XML element */
        if (cur_node->type == XML_ELEMENT_NODE) {
            std::string name = getElementName(cur_node);
            /* Handle each child */
            if (name == "file") {
                model->addFunctionFile(getElementValue(cur_node));
            } else {
                rc = readUnknownElement(cur_node);
                if (rc != 0) return rc;
            }
        }
    }
    return 0;
}

int IOXMLModel::readDataTypes(xmlNode * node,
        model::XModel * model) {
    int rc; /* Return code */
    xmlNode *cur_node = NULL;

    /* Loop through each child of dataTypes */
    for (cur_node = node->children;
            cur_node; cur_node = cur_node->next) {
        /* If node is an XML element */
        if (cur_node->type == XML_ELEMENT_NODE) {
            std::string name = getElementName(cur_node);
        /* Handle each child */
        if (name == "dataType") {
            rc = readDataType(cur_node, model);
            if (rc != 0) return rc;
        } else {
            rc = readUnknownElement(cur_node);
            if (rc != 0) return rc;
        }
        }
    }
    return 0;
}

int IOXMLModel::readDataType(xmlNode * node,
        model::XModel * model) {
    int rc; /* Return code */
    xmlNode *cur_node = NULL;
    model::XADT * xadt = model->addADT();

    /* Loop through each child of dataType */
    for (cur_node = node->children;
            cur_node; cur_node = cur_node->next) {
        /* If node is an XML element */
        if (cur_node->type == XML_ELEMENT_NODE) {
            std::string name = getElementName(cur_node);
            /* Handle each child */
            if (name == "name") {
                xadt->setName(getElementValue(cur_node));
            } else if (name == "description") {
            } else if (name == "variables") {
                rc = readVariables(cur_node, xadt->getVariables());
                if (rc != 0) return rc;
            } else {
                rc = readUnknownElement(cur_node);
                if (rc != 0) return rc;
            }
        }
    }
    return 0;
}

int IOXMLModel::readTimeUnits(xmlNode * node,
        model::XModel * model) {
    int rc; /* Return code */
    xmlNode *cur_node = NULL;

    /* Loop through each child of timeUnits */
    for (cur_node = node->children;
            cur_node; cur_node = cur_node->next) {
        /* If node is an XML element */
        if (cur_node->type == XML_ELEMENT_NODE) {
            std::string name = getElementName(cur_node);
            /* Handle each child */
            if (name == "timeUnit") {
                rc = readTimeUnit(cur_node, model);
                if (rc != 0) return rc;
            } else {
                rc = readUnknownElement(cur_node);
                if (rc != 0) return rc;
            }
        }
    }
    return 0;
}

int IOXMLModel::readTimeUnit(xmlNode * node,
        model::XModel * model) {
    int rc; /* Return code */
    xmlNode *cur_node = NULL;
    model::XTimeUnit * xtimeunit = model->addTimeUnit();

    /* Loop through each child of timeUnit */
    for (cur_node = node->children;
            cur_node; cur_node = cur_node->next) {
            /* If node is an XML element */
        if (cur_node->type == XML_ELEMENT_NODE) {
            std::string name = getElementName(cur_node);
            /* Handle each child */
            if (name == "name") {
                xtimeunit->setName(getElementValue(cur_node));
            } else if (name == "unit") {
                xtimeunit->setUnit(getElementValue(cur_node));
            } else if (name == "period") {
                xtimeunit->setPeriodString(getElementValue(cur_node));
            } else {
                rc = readUnknownElement(cur_node);
                if (rc != 0) return rc;
            }
        }
    }
    return 0;
}

int IOXMLModel::readVariables(xmlNode * node,
        std::vector<model::XVariable*> * variables) {
    int rc; /* Return code */
    xmlNode *cur_node = NULL;

    /* Loop through each child of variables */
    for (cur_node = node->children;
            cur_node; cur_node = cur_node->next) {
        /* If node is an XML element */
        if (cur_node->type == XML_ELEMENT_NODE) {
            std::string name = getElementName(cur_node);
            /* Handle each child */
            if (name == "variable") {
                rc = readVariable(cur_node, variables);
                if (rc != 0) return rc;
            } else {
                rc = readUnknownElement(cur_node);
                if (rc != 0) return rc;
            }
        }
    }
    return 0;
}

int IOXMLModel::readVariable(xmlNode * node,
        std::vector<model::XVariable*> * variables) {
    int rc; /* Return code */
    xmlNode *cur_node = NULL;
    model::XVariable * xvariable = new model::XVariable;
    variables->push_back(xvariable);

    /* Loop through each child of variable */
    for (cur_node = node->children;
            cur_node; cur_node = cur_node->next) {
        /* If node is an XML element */
        if (cur_node->type == XML_ELEMENT_NODE) {
            std::string name = getElementName(cur_node);
            /* Handle each child */
            if (name == "type") {
                xvariable->setType(getElementValue(cur_node));
            } else if (name == "name") {
                xvariable->setName(getElementValue(cur_node));
            } else if (name == "description") {
            } else if (name == "constant") {
                /* Indicate that constant is set */
                xvariable->setConstantSet(true);
                /* Read the string ready to be validated later */
                xvariable->setConstantString(
                        getElementValue(cur_node));
            } else {
                rc = readUnknownElement(cur_node);
                if (rc != 0) return rc;
            }
        }
    }
    return 0;
}

int IOXMLModel::readEnvironment(xmlNode * node,
        model::XModel * model) {
    int rc; /* Return code */
    xmlNode *cur_node = NULL;

    /* Loop through each child of environment */
    for (cur_node = node->children;
            cur_node; cur_node = cur_node->next) {
        /* If node is an XML element */
        if (cur_node->type == XML_ELEMENT_NODE) {
            std::string name = getElementName(cur_node);
            /* Handle each child */
            if (name == "constants")
                rc = readVariables(cur_node, model->getConstants());
            else if (name == "dataTypes")
                rc = readDataTypes(cur_node, model);
            else if (name == "timeUnits")
                rc = readTimeUnits(cur_node, model);
            else if (name == "functionFiles")
                rc = readFunctionFiles(cur_node, model);
            else
                rc = readUnknownElement(cur_node);
            if (rc != 0) return rc;
        }
    }
    return 0;
}


int IOXMLModel::readAgents(xmlNode * node,
        model::XModel * model) {
    int rc; /* Return code */
    xmlNode *cur_node = NULL;

    /* Loop through each child of xagents */
    for (cur_node = node->children;
        cur_node; cur_node = cur_node->next) {
        /* If node is an XML element */
        if (cur_node->type == XML_ELEMENT_NODE) {
            std::string name = getElementName(cur_node);
            /* Handle each child */
            if (name == "xagent") {
                rc = readAgent(cur_node, model);
                if (rc != 0) return rc;
            } else {
                rc = readUnknownElement(cur_node);
                if (rc != 0) return rc;
            }
        }
    }
    return 0;
}

int IOXMLModel::readAgent(xmlNode * node,
        model::XModel * model) {
    int rc; /* Return code */
    xmlNode *cur_node = NULL;
    model::XMachine * xm = model->addAgent();

    /* Loop through each child of xagent */
    for (cur_node = node->children;
        cur_node; cur_node = cur_node->next) {
        /* If node is an XML element */
        if (cur_node->type == XML_ELEMENT_NODE) {
            std::string name = getElementName(cur_node);
            /* Handle each child */
            if (name == "name") {
                xm->setName(getElementValue(cur_node));
            } else if (name == "description") {
            } else if (name == "memory") {
                rc = readVariables(cur_node, xm->getVariables());
            } else if (name == "functions") {
                rc = readTransitions(cur_node, xm);
            } else {
                rc = readUnknownElement(cur_node);
            }
            if (rc != 0) return rc;
        }
    }
    return 0;
}

int IOXMLModel::readInputs(xmlNode * node,
        model::XFunction * xfunction) {
    int rc; /* Return code */
    xmlNode *cur_node = NULL;

    /* Loop through each child of inputs */
    for (cur_node = node->children;
        cur_node; cur_node = cur_node->next) {
        /* If node is an XML element */
        if (cur_node->type == XML_ELEMENT_NODE) {
            std::string name = getElementName(cur_node);
            /* Handle each child */
            if (name == "input") {
                rc = readInput(cur_node, xfunction);
                if (rc != 0) return rc;
            } else {
                rc = readUnknownElement(cur_node);
                if (rc != 0) return rc;
            }
        }
    }
    return 0;
}

int IOXMLModel::readInput(xmlNode * node,
        model::XFunction * xfunction) {
    int rc = 0; /* Return code */
    xmlNode *cur_node = NULL;
    model::XIOput * input = xfunction->addInput();

    /* Loop through each child of input */
    for (cur_node = node->children;
        cur_node; cur_node = cur_node->next) {
        /* If node is an XML element */
        if (cur_node->type == XML_ELEMENT_NODE) {
            std::string name = getElementName(cur_node);
            /* Handle each child */
            if (name == "messageName") {
                input->setMessageName(getElementValue(cur_node));
            } else if (name == "filter") {
                rc = readCondition(cur_node, input->addFilter());
            } else if (name == "sort") {
                rc = readSort(cur_node, input);
            } else if (name == "random") {
                /* Indicate that random is set */
                input->setRandomSet(true);
                /* Read the string ready to be validated later */
                input->setRandomString(getElementValue(cur_node));
            } else {
                rc = readUnknownElement(cur_node);
            }
            if (rc != 0) return rc;
        }
    }
    return 0;
}

int IOXMLModel::readOutputs(xmlNode * node,
        model::XFunction * xfunction) {
    int rc; /* Return code */
    xmlNode *cur_node = NULL;

    /* Loop through each child of outputs */
    for (cur_node = node->children;
        cur_node; cur_node = cur_node->next) {
        /* If node is an XML element */
        if (cur_node->type == XML_ELEMENT_NODE) {
            std::string name = getElementName(cur_node);
            /* Handle each child */
            if (name == "output") {
                rc = readOutput(cur_node, xfunction);
                if (rc != 0) return rc;
            } else {
                rc = readUnknownElement(cur_node);
                if (rc != 0) return rc;
            }
        }
    }
    return 0;
}

int IOXMLModel::readOutput(xmlNode * node,
        model::XFunction * xfunction) {
    int rc; /* Return code */
    xmlNode *cur_node = NULL;
    model::XIOput * output = xfunction->addOutput();

    /* Loop through each child of output */
    for (cur_node = node->children;
        cur_node; cur_node = cur_node->next) {
        /* If node is an XML element */
        if (cur_node->type == XML_ELEMENT_NODE) {
            std::string name = getElementName(cur_node);
            /* Handle each child */
            if (name == "messageName") {
                output->setMessageName(getElementValue(cur_node));
            } else {
                rc = readUnknownElement(cur_node);
                if (rc != 0) return rc;
            }
        }
    }
    return 0;
}

int IOXMLModel::readTransition(xmlNode * node,
        model::XMachine * machine) {
    int rc = 0; /* Return code */
    xmlNode *cur_node = NULL;
    model::XFunction * xfunction = machine->addFunction();

    /* Loop through each child of transition */
    for (cur_node = node->children;
        cur_node; cur_node = cur_node->next) {
        /* If node is an XML element */
        if (cur_node->type == XML_ELEMENT_NODE) {
            std::string name = getElementName(cur_node);
            /* Handle each child */
            if (name == "name")
                xfunction->setName(getElementValue(cur_node));
            else if (name == "currentState")
                xfunction->setCurrentState(getElementValue(cur_node));
            else if (name == "nextState")
                xfunction->setNextState(getElementValue(cur_node));
            else if (name == "condition")
                /* Create condition from function */
                rc = readCondition(cur_node, xfunction->addCondition());
            else if (name == "outputs")
                rc = readOutputs(cur_node, xfunction);
            else if (name == "inputs")
                rc = readInputs(cur_node, xfunction);
            else
                rc = readUnknownElement(cur_node);
            if (rc != 0) return rc;
        }
    }
    return 0;
}

int IOXMLModel::readTransitions(xmlNode * node,
        model::XMachine * machine) {
    int rc; /* Return code */
    xmlNode *cur_node = NULL;

    /* Loop through each child of transitions */
    for (cur_node = node->children;
        cur_node; cur_node = cur_node->next) {
        /* If node is an XML element */
        if (cur_node->type == XML_ELEMENT_NODE) {
            std::string name = getElementName(cur_node);
            /* Handle each child */
            if (name == "function") {
                rc = readTransition(cur_node, machine);
                if (rc != 0) return rc;
            } else {
                rc = readUnknownElement(cur_node);
                if (rc != 0) return rc;
            }
        }
    }
    return 0;
}

int IOXMLModel::readMessages(xmlNode * node,
        model::XModel * model) {
    int rc; /* Return code */
    xmlNode *cur_node = NULL;

    /* Loop through each child of messages */
    for (cur_node = node->children;
        cur_node; cur_node = cur_node->next) {
        /* If node is an XML element */
        if (cur_node->type == XML_ELEMENT_NODE) {
            std::string name = getElementName(cur_node);
            /* Handle each child */
            if (name == "message") {
                rc = readMessage(cur_node, model);
                if (rc != 0) return rc;
            } else {
                rc = readUnknownElement(cur_node);
                if (rc != 0) return rc;
            }
        }
    }
    return 0;
}

int IOXMLModel::readMessage(xmlNode * node,
        model::XModel * model) {
    int rc; /* Return code */
    xmlNode *cur_node = NULL;
    model::XMessage * xmessage = model->addMessage();

    /* Loop through each child of message */
    for (cur_node = node->children;
        cur_node; cur_node = cur_node->next) {
        /* If node is an XML element */
        if (cur_node->type == XML_ELEMENT_NODE) {
            std::string name = getElementName(cur_node);
            /* Handle each child */
            if (name == "name") {
                xmessage->setName(getElementValue(cur_node));
            } else if (name == "description") {
            } else if (name == "variables") {
                rc = readVariables(cur_node, xmessage->getVariables());
                if (rc != 0) return rc;
            } else {
                rc = readUnknownElement(cur_node);
                if (rc != 0) return rc;
            }
        }
    }
    return 0;
}

int IOXMLModel::readSort(xmlNode * node,
        model::XIOput * xioput) {
    int rc; /* Return code */
    xmlNode *cur_node = NULL;
    xioput->setSort(true);

    /* Loop through each child of message */
    for (cur_node = node->children;
        cur_node; cur_node = cur_node->next) {
        /* If node is an XML element */
        if (cur_node->type == XML_ELEMENT_NODE) {
            std::string name = getElementName(cur_node);
            /* Handle each child */
            if (name == "key") {
                xioput->setSortKey(getElementValue(cur_node));
            } else if (name == "order") {
                xioput->setSortOrder(getElementValue(cur_node));
            } else {
                rc = readUnknownElement(cur_node);
                if (rc != 0) return rc;
            }
        }
    }
    return 0;
}

int IOXMLModel::readConditionTime(model::XCondition * xcondition,
        xmlNode *cur_node) {
    int rc;
    /* Update condition to indicate this
     * is a time condition not anything else */
    xcondition->isTime = true;
    xcondition->isValues = false;
    xcondition->isConditions = false;
    xmlNode *cur_node_2 = NULL;
    for (cur_node_2 = cur_node->children;
        cur_node_2; cur_node_2 = cur_node_2->next) {
        /* If node is an XML element */
        if (cur_node_2->type == XML_ELEMENT_NODE) {
            std::string name_2 = getElementName(cur_node_2);
            /* Handle each time element */
            if (name_2 == "period") {
                xcondition->timePeriod = getElementValue(cur_node_2);
            } else if (name_2 == "phase") {
                xcondition->timePhaseVariable = getElementValue(cur_node_2);
            } else if (name_2 == "duration") {
                xcondition->foundTimeDuration = true;
                xcondition->timeDurationString = getElementValue(cur_node_2);
            } else {
                rc = readUnknownElement(cur_node_2);
                if (rc != 0) return rc;
            }
        }
    }

    return 0;
}

int IOXMLModel::readConditionSide(model::XCondition * xcondition,
        model::XCondition ** hsCondition, std::string * hs, bool * hsIsValue,
        bool * hsIsCondition, xmlNode *cur_node) {
    int rc;
    /* Set up and read lhs */
    *hsCondition = new model::XCondition;
    xcondition->tempValue = "";
    rc = readCondition(cur_node, *hsCondition);
    if (rc != 0) return rc;
    /* Handle if lhs is a value or a condition */
    if ((*hsCondition)->tempValue != "") {
        /* lhs is a value */
        *hs = (*hsCondition)->tempValue;
        *hsIsValue = true;
        delete *hsCondition;
        *hsCondition = 0;
    } else {
        /* lhs is a nested condition */
        *hsIsCondition = true;
    }

    return 0;
}

int IOXMLModel::readCondition(xmlNode * node,
        model::XCondition * xc) {
    int rc = 0; /* Return code */
    xmlNode *cur_node = NULL;

    /* Loop through each child of message */
    for (cur_node = node->children;
        cur_node; cur_node = cur_node->next) {
        /* If node is an XML element */
        if (cur_node->type == XML_ELEMENT_NODE) {
            std::string name = getElementName(cur_node);
            /* Handle each child and call appropriate
             * processing function */
            if (name == "not") { xc->isNot = true;
                rc = readCondition(cur_node, xc);
            } else if (name == "time") {
                rc = readConditionTime(xc, cur_node);
            } else if (name == "lhs") {
        rc = readConditionSide(xc, &xc->lhsCondition, &xc->lhs, &xc->lhsIsValue,
                        &xc->lhsIsCondition, cur_node);
            } else if (name == "op") { xc->op = getElementValue(cur_node);
            } else if (name == "rhs") {
        rc = readConditionSide(xc, &xc->rhsCondition, &xc->rhs, &xc->rhsIsValue,
                        &xc->rhsIsCondition, cur_node);
            } else if (name == "value") {
                xc->tempValue = getElementValue(cur_node);
            } else {
                rc = readUnknownElement(cur_node);
            }
            if (rc != 0) return rc;
        }
    }
    return 0;
}

}}}  // namespace flame::io::xml
