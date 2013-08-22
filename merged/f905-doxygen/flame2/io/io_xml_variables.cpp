/*!
 * \file flame2/io/io_xml_variables.cpp
 * \author Simon Coakley
 * \date 2012
 * \copyright Copyright (c) 2013 STFC Rutherford Appleton Laboratory
 * \copyright Copyright (c) 2013 University of Sheffield
 * \copyright GNU Lesser General Public License
 * \brief IOXMLVariables: reading of variables from XML
 */
#include <libxml/parser.h>
#include <libxml/tree.h>
#include <string>
#include <boost/lexical_cast.hpp>
#include "flame2/config.hpp"
#include "io_xml_variables.hpp"

namespace flame { namespace io { namespace xml {

void IOXMLVariables::readVariables(xmlNode * node,
    boost::ptr_vector<model::XVariable> * variables) {
  xmlNode *cur_node = NULL;

  /* Loop through each child of variables */
  for (cur_node = node->children;
      cur_node; cur_node = cur_node->next) {
    /* If node is an XML element */
    if (cur_node->type == XML_ELEMENT_NODE) {
      std::string name = ioxmlelement.getElementName(cur_node);
      /* Handle each child */
      if (name == "variable")
        readVariable(cur_node, variables);
      else
        ioxmlelement.readUnknownElement(cur_node);
    }
  }
}

void IOXMLVariables::readVariable(xmlNode * node,
    boost::ptr_vector<model::XVariable> * variables) {
  xmlNode *cur_node = NULL;
  model::XVariable * xvariable = new model::XVariable;
  variables->push_back(xvariable);

  /* Loop through each child of variable */
  for (cur_node = node->children;
      cur_node; cur_node = cur_node->next) {
    /* If node is an XML element */
    if (cur_node->type == XML_ELEMENT_NODE) {
      std::string name = ioxmlelement.getElementName(cur_node);
      /* Handle each child */
      if (name == "type") {
        xvariable->setType(ioxmlelement.getElementValue(cur_node));
      } else if (name == "name") {
        xvariable->setName(ioxmlelement.getElementValue(cur_node));
      } else if (name == "description") {
      } else if (name == "constant") {
        /* Indicate that constant is set */
        xvariable->setConstantSet(true);
        /* Read the string ready to be validated later */
        xvariable->setConstantString(
            ioxmlelement.getElementValue(cur_node));
      } else {
        ioxmlelement.readUnknownElement(cur_node);
      }
    }
  }
}

}}}  // namespace flame::io::xml
