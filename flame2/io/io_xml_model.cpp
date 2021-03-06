/*!
 * \file flame2/io/io_xml_model.cpp
 * \author Simon Coakley
 * \date 2012
 * \copyright Copyright (c) 2012 STFC Rutherford Appleton Laboratory
 * \copyright Copyright (c) 2012 University of Sheffield
 * \copyright GNU Lesser General Public License
 * \brief IOXMLModel: reading of model XML file
 */
#include <boost/algorithm/string/predicate.hpp>
#include <boost/filesystem.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/ptr_container/ptr_vector.hpp>
#include <string>
#include <vector>
#include <cstdio>
#include "flame2/config.hpp"
#include "flame2/exceptions/io.hpp"
#include "io_xml_model.hpp"

namespace model = flame::model;
namespace exc = flame::exceptions;

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

void IOXMLModel::validateXMLModelRootElement(
    xmlNode *root_element, std::string file_name, xmlDoc *doc) {
  /* Catch error if no root called xmodel */
  if (getElementName(root_element) != "xmodel") {
    xmlFreeDoc(doc);
    throw exc::invalid_model_file(std::string(
        "Model file does not have root called 'xmodel': ") +
        file_name);
  }

  /* Catch error if version is not 2 */
  xmlChar * version_ptr = xmlGetProp(root_element, (const xmlChar*)"version");
  std::string version = reinterpret_cast<const char*>(version_ptr);
  xmlFree(version_ptr);
  if (version != "2") {
    xmlFreeDoc(doc);
    throw exc::invalid_model_file(std::string(
        "Model file is not 'xmodel' version 2: ") + file_name);
  }
}

void IOXMLModel::readXMLModel(std::string file_name, model::XModel * model) {
  /* Directory of the model file */
  std::string directory;
  xmlDoc *doc = NULL;
  xmlNode *root_element = NULL;

  /* Find file directory to help open any submodels */
  boost::filesystem::path filePath(file_name);
  directory = filePath.parent_path().string();
  directory.append("/");

  /* Print out diagnostics */
#ifndef TESTBUILD
  fprintf(stdout, "Reading file: %s\n", file_name.c_str());
#endif
  /* Save absolute path to check the file is not read again */
  model->setPath(boost::filesystem::absolute(filePath).string());

  // Check if file exists
  if (!boost::filesystem::exists(file_name)) {
    xmlFreeDoc(doc);
    throw exc::inaccessable_file(std::string(
        "Error: Model file cannot be opened: ") + file_name);
  }

  /* Parse the file and get the DOM */
  doc = xmlReadFile(file_name.c_str(), NULL, 0);

  /* Check if file opened successfully */
  if (doc == NULL)
    /* Return error if the file was not successfully parsed */
    throw exc::unparseable_file(std::string(
        "Error: Model file cannot be parsed: ") + file_name);

  /*Get the root element node */
  root_element = xmlDocGetRootElement(doc);

  validateXMLModelRootElement(root_element, file_name, doc);
  readModelElements(root_element, model, directory, doc);

  xmlFreeDoc(doc);
}

void IOXMLModel::readModelElements(xmlNode *root_element, model::XModel * model,
    std::string directory, xmlDoc *doc) {
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
        readIncludedModels(cur_node, directory, model, doc);
      else if (name == "environment")
        readEnvironment(cur_node, model);
      else if (name == "agents")
        readAgents(cur_node, model);
      else if (name == "messages")
        readMessages(cur_node, model);
      else
        readUnknownElement(cur_node);
    }
  }
}

void IOXMLModel::readUnknownElement(xmlNode * node) {
  std::string error = "Warning: Model file has unknown element '";
  error.append(boost::lexical_cast<std::string>(node->name));
  error.append("' on line ");
  error.append(boost::lexical_cast<std::string>(node->line));
  fprintf(stderr, "%s\n", error.c_str());
}

void IOXMLModel::readIncludedModels(xmlNode * node,
    std::string directory, model::XModel * model, xmlDoc *doc) {
  xmlNode *cur_node = NULL;

  /* Loop through each child of models */
  for (cur_node = node->children;
      cur_node; cur_node = cur_node->next) {
    /* If node is an XML element */
    if (cur_node->type == XML_ELEMENT_NODE) {
      std::string name = getElementName(cur_node);
      /* Handle each child */
      if (name == "model")
        readIncludedModel(cur_node, directory, model, doc);
      else
        readUnknownElement(cur_node);
    }
  }
}

void IOXMLModel::readIncludedModelValidate(std::string directory,
    std::string fileName, model::XModel * model, bool enable, xmlDoc *doc) {
  /* If included model is enabled */
  if (enable) {
    /* Check file name ends in '.xml' or '.XML' */
    if (!boost::algorithm::ends_with(fileName, ".xml") &&
        !boost::algorithm::ends_with(fileName, ".XML")) {
      xmlFreeDoc(doc);
      throw exc::flame_io_exception(
        std::string("Included model file does not end in '.xml': ") + fileName);
    }

    /* Append file name to current model file directory */
    fileName = directory.append(fileName);

    /* Check sub model is not a duplicate */
    if (!model->addIncludedModel(
        boost::filesystem::absolute(fileName).string())) {
      xmlFreeDoc(doc);
      throw exc::flame_io_exception(std::string(
          "Error: Included model is a duplicate: ") +
          fileName);
    }

    /* Read model file... */
    try { readXMLModel(fileName, model); }
    catch(const exc::inaccessable_file& E) {
      xmlFreeDoc(doc);
      throw exc::inaccessable_file(std::string(
          "Error: Submodel file cannot be opened: ") + fileName);
    }
  }
}

void IOXMLModel::readIncludedModel(xmlNode * node,
    std::string directory, model::XModel * model, xmlDoc *doc) {
  xmlNode *cur_node = NULL;
  std::string fileName;
  bool enable = true;

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
          xmlFreeDoc(doc);
          throw exc::flame_io_exception(
              std::string("Error: Included model has invalid enabled value ") +
              enabledString);
        }
      } else {
        readUnknownElement(cur_node);
      }
    }
  }

  /* Handle enabled models */
  readIncludedModelValidate(directory, fileName, model, enable, doc);
}


void IOXMLModel::readFunctionFiles(xmlNode * node,
    model::XModel * model) {
  xmlNode *cur_node = NULL;

  /* Loop through each child of functionFiles */
  for (cur_node = node->children;
      cur_node; cur_node = cur_node->next) {
    /* If node is an XML element */
    if (cur_node->type == XML_ELEMENT_NODE) {
      std::string name = getElementName(cur_node);
      /* Handle each child */
      if (name == "file")
        model->addFunctionFile(getElementValue(cur_node));
      else
        readUnknownElement(cur_node);
    }
  }
}

void IOXMLModel::readDataTypes(xmlNode * node,
    model::XModel * model) {
  xmlNode *cur_node = NULL;

  /* Loop through each child of dataTypes */
  for (cur_node = node->children;
      cur_node; cur_node = cur_node->next) {
    /* If node is an XML element */
    if (cur_node->type == XML_ELEMENT_NODE) {
      std::string name = getElementName(cur_node);
      /* Handle each child */
      if (name == "dataType")
        readDataType(cur_node, model);
      else
        readUnknownElement(cur_node);
    }
  }
}

void IOXMLModel::readDataType(xmlNode * node,
    model::XModel * model) {
  xmlNode *cur_node = NULL;
  model::XADT * xadt = model->addADT();

  /* Loop through each child of dataType */
  for (cur_node = node->children;
      cur_node; cur_node = cur_node->next) {
    /* If node is an XML element */
    if (cur_node->type == XML_ELEMENT_NODE) {
      std::string name = getElementName(cur_node);
      /* Handle each child */
      if (name == "name")
        xadt->setName(getElementValue(cur_node));
      else if (name == "description") {}
      else if (name == "variables")
        readVariables(cur_node, xadt->getVariables());
      else
        readUnknownElement(cur_node);
    }
  }
}

void IOXMLModel::readTimeUnits(xmlNode * node,
    model::XModel * model) {
  xmlNode *cur_node = NULL;

  /* Loop through each child of timeUnits */
  for (cur_node = node->children;
      cur_node; cur_node = cur_node->next) {
    /* If node is an XML element */
    if (cur_node->type == XML_ELEMENT_NODE) {
      std::string name = getElementName(cur_node);
      /* Handle each child */
      if (name == "timeUnit")
        readTimeUnit(cur_node, model);
      else
        readUnknownElement(cur_node);
    }
  }
}

void IOXMLModel::readTimeUnit(xmlNode * node,
    model::XModel * model) {
  xmlNode *cur_node = NULL;
  model::XTimeUnit * xtimeunit = new model::XTimeUnit;

  /* Loop through each child of timeUnit */
  for (cur_node = node->children;
      cur_node; cur_node = cur_node->next) {
    /* If node is an XML element */
    if (cur_node->type == XML_ELEMENT_NODE) {
      std::string name = getElementName(cur_node);
      /* Handle each child */
      if (name == "name")
        xtimeunit->setName(getElementValue(cur_node));
      else if (name == "unit")
        xtimeunit->setUnit(getElementValue(cur_node));
      else if (name == "period")
        xtimeunit->setPeriodString(getElementValue(cur_node));
      else
        readUnknownElement(cur_node);
    }
  }

  model->addTimeUnit(xtimeunit);
}

void IOXMLModel::readVariables(xmlNode * node,
    boost::ptr_vector<model::XVariable> * variables) {
  xmlNode *cur_node = NULL;

  /* Loop through each child of variables */
  for (cur_node = node->children;
      cur_node; cur_node = cur_node->next) {
    /* If node is an XML element */
    if (cur_node->type == XML_ELEMENT_NODE) {
      std::string name = getElementName(cur_node);
      /* Handle each child */
      if (name == "variable")
        readVariable(cur_node, variables);
      else
        readUnknownElement(cur_node);
    }
  }
}

void IOXMLModel::readVariable(xmlNode * node,
    boost::ptr_vector<model::XVariable> * variables) {
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
        readUnknownElement(cur_node);
      }
    }
  }
}

void IOXMLModel::readEnvironment(xmlNode * node,
    model::XModel * model) {
  xmlNode *cur_node = NULL;

  /* Loop through each child of environment */
  for (cur_node = node->children;
      cur_node; cur_node = cur_node->next) {
    /* If node is an XML element */
    if (cur_node->type == XML_ELEMENT_NODE) {
      std::string name = getElementName(cur_node);
      /* Handle each child */
      if (name == "constants")
        readVariables(cur_node, model->getConstants());
      else if (name == "dataTypes")
        readDataTypes(cur_node, model);
      else if (name == "timeUnits")
        readTimeUnits(cur_node, model);
      else if (name == "functionFiles")
        readFunctionFiles(cur_node, model);
      else
        readUnknownElement(cur_node);
    }
  }
}


void IOXMLModel::readAgents(xmlNode * node,
    model::XModel * model) {
  xmlNode *cur_node = NULL;

  /* Loop through each child of xagents */
  for (cur_node = node->children;
      cur_node; cur_node = cur_node->next) {
    /* If node is an XML element */
    if (cur_node->type == XML_ELEMENT_NODE) {
      std::string name = getElementName(cur_node);
      /* Handle each child */
      if (name == "xagent")
        readAgent(cur_node, model);
      else
        readUnknownElement(cur_node);
    }
  }
}

void IOXMLModel::readAgent(xmlNode * node,
    model::XModel * model) {
  xmlNode *cur_node = NULL;
  model::XMachine * xm = 0;

  /* Loop through each child of xagent */
  for (cur_node = node->children;
      cur_node; cur_node = cur_node->next) {
    /* If node is an XML element */
    if (cur_node->type == XML_ELEMENT_NODE) {
      std::string name = getElementName(cur_node);
      /* Handle each child */
      if (name == "name")
        xm = model->addAgent(getElementValue(cur_node));
      else if (name == "description") {}
      else if (name == "memory")
        readVariables(cur_node, xm->getVariables());
      else if (name == "functions")
        readTransitions(cur_node, xm);
      else
        readUnknownElement(cur_node);
    }
  }
}

void IOXMLModel::readInputs(xmlNode * node,
    model::XFunction * xfunction) {
  xmlNode *cur_node = NULL;

  /* Loop through each child of inputs */
  for (cur_node = node->children;
      cur_node; cur_node = cur_node->next) {
    /* If node is an XML element */
    if (cur_node->type == XML_ELEMENT_NODE) {
      std::string name = getElementName(cur_node);
      /* Handle each child */
      if (name == "input")
        readInput(cur_node, xfunction);
      else
        readUnknownElement(cur_node);
    }
  }
}

void IOXMLModel::readInput(xmlNode * node,
    model::XFunction * xfunction) {
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
        readCondition(cur_node, input->addFilter());
      } else if (name == "sort") {
        readSort(cur_node, input);
      } else if (name == "random") {
        /* Indicate that random is set */
        input->setRandomSet(true);
        /* Read the string ready to be validated later */
        input->setRandomString(getElementValue(cur_node));
      } else {
        readUnknownElement(cur_node);
      }
    }
  }
}

void IOXMLModel::readOutputs(xmlNode * node,
    model::XFunction * xfunction) {
  xmlNode *cur_node = NULL;

  /* Loop through each child of outputs */
  for (cur_node = node->children;
      cur_node; cur_node = cur_node->next) {
    /* If node is an XML element */
    if (cur_node->type == XML_ELEMENT_NODE) {
      std::string name = getElementName(cur_node);
      /* Handle each child */
      if (name == "output")
        readOutput(cur_node, xfunction);
      else
        readUnknownElement(cur_node);
    }
  }
}

void IOXMLModel::readOutput(xmlNode * node,
    model::XFunction * xfunction) {
  xmlNode *cur_node = NULL;
  model::XIOput * output = xfunction->addOutput();

  /* Loop through each child of output */
  for (cur_node = node->children;
      cur_node; cur_node = cur_node->next) {
    /* If node is an XML element */
    if (cur_node->type == XML_ELEMENT_NODE) {
      std::string name = getElementName(cur_node);
      /* Handle each child */
      if (name == "messageName")
        output->setMessageName(getElementValue(cur_node));
      else
        readUnknownElement(cur_node);
    }
  }
}

void IOXMLModel::readTransition(xmlNode * node,
    model::XMachine * machine) {
  xmlNode *cur_node = NULL;
  model::XFunction * xfunction = machine->addFunction();

  // Set memory access information to be unavailable
  xfunction->setMemoryAccessInfoAvailable(false);

  /* Loop through each child of transition */
  for (cur_node = node->children;
      cur_node; cur_node = cur_node->next) {
    /* If node is an XML element */
    if (cur_node->type == XML_ELEMENT_NODE) {
      std::string name = getElementName(cur_node);
      /* Handle each child */
      if (name == "name")
        xfunction->setName(getElementValue(cur_node));
      else if (name == "description") {}
      else if (name == "currentState")
        xfunction->setCurrentState(getElementValue(cur_node));
      else if (name == "nextState")
        xfunction->setNextState(getElementValue(cur_node));
      else if (name == "condition")
        /* Create condition from function */
        readCondition(cur_node, xfunction->addCondition());
      else if (name == "outputs") readOutputs(cur_node, xfunction);
      else if (name == "inputs") readInputs(cur_node, xfunction);
      else if (name == "memoryAccess")
        readMemoryAccess(cur_node, xfunction);
      else
        readUnknownElement(cur_node);
    }
  }
}

void IOXMLModel::readTransitions(xmlNode * node,
    model::XMachine * machine) {
  xmlNode *cur_node = NULL;

  /* Loop through each child of transitions */
  for (cur_node = node->children;
      cur_node; cur_node = cur_node->next) {
    /* If node is an XML element */
    if (cur_node->type == XML_ELEMENT_NODE) {
      std::string name = getElementName(cur_node);
      /* Handle each child */
      if (name == "function")
        readTransition(cur_node, machine);
      else
        readUnknownElement(cur_node);
    }
  }
}

void IOXMLModel::readMessages(xmlNode * node,
    model::XModel * model) {
  xmlNode *cur_node = NULL;

  /* Loop through each child of messages */
  for (cur_node = node->children;
      cur_node; cur_node = cur_node->next) {
    /* If node is an XML element */
    if (cur_node->type == XML_ELEMENT_NODE) {
      std::string name = getElementName(cur_node);
      /* Handle each child */
      if (name == "message")
        readMessage(cur_node, model);
      else
        readUnknownElement(cur_node);
    }
  }
}

void IOXMLModel::readMessage(xmlNode * node,
    model::XModel * model) {
  xmlNode *cur_node = NULL;
  model::XMessage * xmessage = model->addMessage();

  /* Loop through each child of message */
  for (cur_node = node->children;
      cur_node; cur_node = cur_node->next) {
    /* If node is an XML element */
    if (cur_node->type == XML_ELEMENT_NODE) {
      std::string name = getElementName(cur_node);
      /* Handle each child */
      if (name == "name")
        xmessage->setName(getElementValue(cur_node));
      else if (name == "description") {}
      else if (name == "variables")
        readVariables(cur_node, xmessage->getVariables());
      else
        readUnknownElement(cur_node);
    }
  }
}

void IOXMLModel::readSort(xmlNode * node,
    model::XIOput * xioput) {
  xmlNode *cur_node = NULL;
  xioput->setSort(true);

  /* Loop through each child of message */
  for (cur_node = node->children;
      cur_node; cur_node = cur_node->next) {
    /* If node is an XML element */
    if (cur_node->type == XML_ELEMENT_NODE) {
      std::string name = getElementName(cur_node);
      /* Handle each child */
      if (name == "key")
        xioput->setSortKey(getElementValue(cur_node));
      else if (name == "order")
        xioput->setSortOrder(getElementValue(cur_node));
      else
        readUnknownElement(cur_node);
    }
  }
}

void IOXMLModel::readConditionTime(model::XCondition * xcondition,
    xmlNode *cur_node) {
  /* Update condition to indicate this
   * is a time condition not anything else */
  xcondition->setIsTime(true);
  xcondition->setIsValues(false);
  xcondition->setIsConditions(false);
  xmlNode *cur_node_2 = NULL;
  for (cur_node_2 = cur_node->children;
      cur_node_2; cur_node_2 = cur_node_2->next) {
    /* If node is an XML element */
    if (cur_node_2->type == XML_ELEMENT_NODE) {
      std::string name_2 = getElementName(cur_node_2);
      /* Handle each time element */
      if (name_2 == "period") {
        xcondition->setTimePeriod(getElementValue(cur_node_2));
      } else if (name_2 == "phase") {
        xcondition->setTimePhaseVariable(getElementValue(cur_node_2));
      } else if (name_2 == "duration") {
        xcondition->setFoundTimeDuration(true);
        xcondition->setTimeDurationString(getElementValue(cur_node_2));
      } else {
        readUnknownElement(cur_node_2);
      }
    }
  }
}

void IOXMLModel::readConditionLhs(model::XCondition * xc,
    xmlNode *cur_node) {
  /* Set up and read lhs */
  xc->setLhsCondition(new model::XCondition);
  xc->setTempValue("");
  readCondition(cur_node, xc->lhsCondition());
  /* Handle if lhs is a value or a condition */
  if (xc->lhsCondition()->tempValue() != "") {
    /* lhs is a value */
    xc->setLhs(xc->lhsCondition()->tempValue());
    xc->setLhsIsValue(true);
    delete xc->lhsCondition();
    xc->setLhsCondition(0);
  } else {
    /* lhs is a nested condition */
    xc->setLhsIsCondition(true);
  }
}

void IOXMLModel::readConditionRhs(model::XCondition * xc,
    xmlNode *cur_node) {
  /* Set up and read rhs */
  xc->setRhsCondition(new model::XCondition);
  xc->setTempValue("");
  readCondition(cur_node, xc->rhsCondition());
  /* Handle if rhs is a value or a condition */
  if (xc->rhsCondition()->tempValue() != "") {
    /* rhs is a value */
    xc->setRhs(xc->rhsCondition()->tempValue());
    xc->setRhsIsValue(true);
    delete xc->rhsCondition();
    xc->setRhsCondition(0);
  } else {
    /* rhs is a nested condition */
    xc->setRhsIsCondition(true);
  }
}

void IOXMLModel::readCondition(xmlNode * node,
    model::XCondition * xc) {
  xmlNode *cur_node = NULL;

  /* Loop through each child of message */
  for (cur_node = node->children;
      cur_node; cur_node = cur_node->next) {
    /* If node is an XML element */
    if (cur_node->type == XML_ELEMENT_NODE) {
      std::string name = getElementName(cur_node);
      /* Handle each child and call appropriate
       * processing function */
      if (name == "not") { xc->setIsNot(true);
      readCondition(cur_node, xc);
      } else if (name == "time") {
        readConditionTime(xc, cur_node);
      } else if (name == "lhs") {
        readConditionLhs(xc, cur_node);
      } else if (name == "op") { xc->setOp(getElementValue(cur_node));
      } else if (name == "rhs") {
        readConditionRhs(xc, cur_node);
      } else if (name == "value") {
        xc->setTempValue(getElementValue(cur_node));
      } else {
        readUnknownElement(cur_node);
      }
    }
  }
}

void IOXMLModel::readMemoryAccessVariables(xmlNode * node,
    std::vector<std::string> * variables) {
  xmlNode *cur_node = NULL;

  /* Loop through each child of message */
  for (cur_node = node->children;
      cur_node; cur_node = cur_node->next) {
    /* If node is an XML element */
    if (cur_node->type == XML_ELEMENT_NODE) {
      std::string name = getElementName(cur_node);
      /* Handle each child and call appropriate
       * processing function */
      if (name == "variableName") {
        variables->push_back(getElementValue(cur_node));
      } else {
        readUnknownElement(cur_node);
      }
    }
  }
}

void IOXMLModel::readMemoryAccess(
    xmlNode * node, model::XFunction * xfunction) {
  xmlNode *cur_node = NULL;

  // Set memory access information to be available
  xfunction->setMemoryAccessInfoAvailable(true);

  /* Loop through each child of message */
  for (cur_node = node->children;
      cur_node; cur_node = cur_node->next) {
    /* If node is an XML element */
    if (cur_node->type == XML_ELEMENT_NODE) {
      std::string name = getElementName(cur_node);
      /* Handle each child and call appropriate
       * processing function */
      if (name == "readOnly") {
        readMemoryAccessVariables(cur_node,
            xfunction->getReadOnlyVariables());
      } else if (name == "readWrite") {
        readMemoryAccessVariables(cur_node,
            xfunction->getReadWriteVariables());
      } else {
        readUnknownElement(cur_node);
      }
    }
  }
}

}}}  // namespace flame::io::xml
