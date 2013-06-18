/*!
 * \file flame2/io/io_xml_element.cpp
 * \author Simon Coakley
 * \date 2012
 * \copyright Copyright (c) 2013 STFC Rutherford Appleton Laboratory
 * \copyright Copyright (c) 2013 University of Sheffield
 * \copyright GNU Lesser General Public License
 * \brief IOXMLElement: access element info
 */
#include <libxml/parser.h>
#include <libxml/tree.h>
#include <boost/lexical_cast.hpp>
#include "flame2/config.hpp"
#include "io_xml_element.hpp"

namespace flame { namespace io { namespace xml {

std::string IOXMLElement::getElementName(xmlNode * node) {
  std::string s = reinterpret_cast<const char*>(node->name);
  return s;
}

std::string IOXMLElement::getElementValue(xmlNode * node) {
  std::string s = reinterpret_cast<const char*>(node->children->content);
  return s;
}

void IOXMLElement::readUnknownElement(xmlNode * node) {
  std::string error = "Warning: Model file has unknown element '";
  error.append(boost::lexical_cast<std::string>(node->name));
  error.append("' on line ");
  error.append(boost::lexical_cast<std::string>(node->line));
  fprintf(stderr, "%s\n", error.c_str());
}

}}}  // namespace flame::io::xml
