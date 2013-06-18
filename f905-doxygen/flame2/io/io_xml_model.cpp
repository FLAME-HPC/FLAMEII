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

void IOXMLModel::validateXMLModelRootElement(
    xmlNode *root_element, std::string file_name, xmlDoc *doc) {
  /* Catch error if no root called xmodel */
  if (ioxmlelement.getElementName(root_element) != "xmodel") {
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
    throw exc::inaccessible_file(std::string(
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
      std::string name = ioxmlelement.getElementName(cur_node);
      if (name == "name")
        model->setName(ioxmlelement.getElementValue(cur_node));
      /* Version/Author/Description not read */
      else if (name == "version") {}
      else if (name == "author") {}
      else if (name == "description") {}
      else if (name == "models")
        readIncludedModels(cur_node, directory, model, doc);
      else if (name == "environment")
        readEnvironment(cur_node, model);
      else if (name == "agents")
        ioxmlagents.readAgents(cur_node, model);
      else if (name == "messages")
        readMessages(cur_node, model);
      else
        ioxmlelement.readUnknownElement(cur_node);
    }
  }
}

void IOXMLModel::readIncludedModels(xmlNode * node,
    std::string directory, model::XModel * model, xmlDoc *doc) {
  xmlNode *cur_node = NULL;

  /* Loop through each child of models */
  for (cur_node = node->children;
      cur_node; cur_node = cur_node->next) {
    /* If node is an XML element */
    if (cur_node->type == XML_ELEMENT_NODE) {
      std::string name = ioxmlelement.getElementName(cur_node);
      /* Handle each child */
      if (name == "model")
        readIncludedModel(cur_node, directory, model, doc);
      else
        ioxmlelement.readUnknownElement(cur_node);
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
    catch(const exc::inaccessible_file& E) {
      xmlFreeDoc(doc);
      throw exc::inaccessible_file(std::string(
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
      std::string name = ioxmlelement.getElementName(cur_node);
      /* Handle each child */
      if (name == "file") fileName = ioxmlelement.getElementValue(cur_node);
      else if (name == "enabled") {
        std::string enabledString = ioxmlelement.getElementValue(cur_node);
        if (enabledString == "true") { enable = true;
        } else if (enabledString == "false") { enable = false;
        } else {
          xmlFreeDoc(doc);
          throw exc::flame_io_exception(
              std::string("Error: Included model has invalid enabled value ") +
              enabledString);
        }
      } else {
        ioxmlelement.readUnknownElement(cur_node);
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
      std::string name = ioxmlelement.getElementName(cur_node);
      /* Handle each child */
      if (name == "file")
        model->addFunctionFile(ioxmlelement.getElementValue(cur_node));
      else
        ioxmlelement.readUnknownElement(cur_node);
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
      std::string name = ioxmlelement.getElementName(cur_node);
      /* Handle each child */
      if (name == "dataType")
        readDataType(cur_node, model);
      else
        ioxmlelement.readUnknownElement(cur_node);
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
      std::string name = ioxmlelement.getElementName(cur_node);
      /* Handle each child */
      if (name == "name")
        xadt->setName(ioxmlelement.getElementValue(cur_node));
      else if (name == "description") {}
      else if (name == "variables")
        ioxmlvariables.readVariables(cur_node, xadt->getVariables());
      else
        ioxmlelement.readUnknownElement(cur_node);
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
      std::string name = ioxmlelement.getElementName(cur_node);
      /* Handle each child */
      if (name == "timeUnit")
        readTimeUnit(cur_node, model);
      else
        ioxmlelement.readUnknownElement(cur_node);
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
      std::string name = ioxmlelement.getElementName(cur_node);
      /* Handle each child */
      if (name == "name")
        xtimeunit->setName(ioxmlelement.getElementValue(cur_node));
      else if (name == "unit")
        xtimeunit->setUnit(ioxmlelement.getElementValue(cur_node));
      else if (name == "period")
        xtimeunit->setPeriodString(ioxmlelement.getElementValue(cur_node));
      else
        ioxmlelement.readUnknownElement(cur_node);
    }
  }

  model->addTimeUnit(xtimeunit);
}

void IOXMLModel::readEnvironment(xmlNode * node,
    model::XModel * model) {
  xmlNode *cur_node = NULL;

  /* Loop through each child of environment */
  for (cur_node = node->children;
      cur_node; cur_node = cur_node->next) {
    /* If node is an XML element */
    if (cur_node->type == XML_ELEMENT_NODE) {
      std::string name = ioxmlelement.getElementName(cur_node);
      /* Handle each child */
      if (name == "constants")
        ioxmlvariables.readVariables(cur_node, model->getConstants());
      else if (name == "dataTypes")
        readDataTypes(cur_node, model);
      else if (name == "timeUnits")
        readTimeUnits(cur_node, model);
      else if (name == "functionFiles")
        readFunctionFiles(cur_node, model);
      else
        ioxmlelement.readUnknownElement(cur_node);
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
      std::string name = ioxmlelement.getElementName(cur_node);
      /* Handle each child */
      if (name == "message")
        readMessage(cur_node, model);
      else
        ioxmlelement.readUnknownElement(cur_node);
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
      std::string name = ioxmlelement.getElementName(cur_node);
      /* Handle each child */
      if (name == "name")
        xmessage->setName(ioxmlelement.getElementValue(cur_node));
      else if (name == "description") {}
      else if (name == "variables")
        ioxmlvariables.readVariables(cur_node, xmessage->getVariables());
      else
        ioxmlelement.readUnknownElement(cur_node);
    }
  }
}

}}}  // namespace flame::io::xml
