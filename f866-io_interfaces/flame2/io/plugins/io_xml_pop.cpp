/*!
 * \file flame2/io/plugins/io_xml_pop.cpp
 * \author Simon Coakley
 * \date 2012
 * \copyright Copyright (c) 2012 STFC Rutherford Appleton Laboratory
 * \copyright Copyright (c) 2012 University of Sheffield
 * \copyright GNU Lesser General Public License
 * \brief IOXMLPop: reading and writing of population XML file
 */
#include <libxml/xmlreader.h>
#include <libxml/xmlwriter.h>
#include <boost/filesystem.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/variant.hpp>
#include <string>
#include <vector>
#include <set>
#include <cstdio>
#include <utility>
#include "io_xml_pop.hpp"

//#include "flame2/config.hpp"  // Needed?

// enable test build
#define TESTBUILD 1

namespace flame { namespace io {

typedef std::set< std::pair<std::string, std::string> > StringPairSet;
typedef std::set<std::string> StringSet;

IOXMLPop::IOXMLPop()
    : xml_pop_path_is_set(false), addInt(0), addDouble(0) {
}

// This method is empty because you can't (without a lot of difficulty)
// write xml row-wise
void IOXMLPop::writePop(std::string const& agent_name,
    std::string const& var_name, size_t size, void * ptr) {

  // get agent
  AgentMemoryArrays::iterator ait;
  ait = agentMemoryArrays_.find(agent_name);
  if (ait == agentMemoryArrays_.end())
    ait = agentMemoryArrays_.insert(
        std::make_pair(agent_name, VarPtrArrayMap())).first;
  // get variable
  VarPtrArrayMap * vpam = &(*ait).second;
  VarPtrArrayMap::iterator vit;
  vit = vpam->find(var_name);
  if (vit == vpam->end())
    vit = vpam->insert(std::make_pair(var_name, PtrArray())).first;
  // set pointer to array and size
  (*vit).second.first = ptr;
  (*vit).second.second = size;
}

void IOXMLPop::initialiseData() {
  // Write out xml start and environment data when implemented
}

void IOXMLPop::writeAgents() {
  AgentMemory::iterator ait;
  // for each agent type
  for (ait = agentMemory_.begin(); ait != agentMemory_.end(); ++ait) {
    std::string agent_name = ait->first;
    VarPtrArrayMap * vpam = &(*agentMemoryArrays_.find(agent_name)).second;
    size_t size = 0;
    // get a size from first variable if exists
    if (vpam->size() > 0) size = vpam->begin()->second.second;
    // for each agent instance
    size_t ii;
    for (ii = 0; ii < size; ++ii) {
      // open root tag
      writer_.writeXMLTag("xagent");
      // write agent name
      writer_.writeXMLTag("name", agent_name);
      // for each variable
      VarVec::iterator vit;
      for (vit = ait->second.begin(); vit != ait->second.end(); ++vit) {
        std::string var_name = vit->second;
        VarPtrArrayMap::iterator vpamit = vpam->find(var_name);
        // find var type
        VarVec::iterator vvit;
        std::string var_type;
        for (vvit = (*agentMemory_.find(agent_name)).second.begin();
            vvit != (*agentMemory_.find(agent_name)).second.end(); ++vvit)
          if ((*vvit).second == var_name) var_type = (*vvit).first;
        if (var_type == "int") writer_.writeXMLTag(var_name,
            *(static_cast<int*>(vpamit->second.first)+ii));
        if (var_type == "double") writer_.writeXMLTag(var_name,
            *(static_cast<double*>(vpamit->second.first)+ii));
      }
      // close the element named xagent
      writer_.writeXMLEndTag();
    }
  }
}

void IOXMLPop::finaliseData() {
  /* The xml text writer */
  xmlTextWriterPtr writer;

  /* Check a path has been set */
  if (!xmlPopPathIsSet()) throw std::runtime_error("Path not set");

  std::string file_name = xml_pop_path;
  file_name.append(boost::lexical_cast<std::string>(iteration_));
  file_name.append(".xml");

#ifndef TESTBUILD
  printf("Writing file: %s\n", file_name.c_str());
#endif

  /* Open file to write to, with no compression */
  writer = xmlNewTextWriterFilename(file_name.c_str(), 0);
  if (writer == NULL)
    throw std::runtime_error(
        "Could not open xml population file for writing");
  /* Write tags on new lines */
  xmlTextWriterSetIndent(writer, 1);

  // set up xml writer
  writer_.setWriterPtr(writer);

  /* Open root tag */
  writer_.writeXMLTag("states");

  /* Write itno tag with iteration number */
  writer_.writeXMLTag("itno", static_cast<int>(iteration_));

  // Write agent memory out
  writeAgents();

  /* End xml file, automatically ends states tag */
  writer_.endXMLDoc();

  /* Free the xml writer */
  xmlFreeTextWriter(writer);

  agentMemoryArrays_.clear();
}

void IOXMLPop::readPop(std::string file_name,
    void (*paddInt)(std::string const&, std::string const&, int),
    void (*paddDouble)(std::string const&, std::string const&, double)) {
  addInt = paddInt;
  addDouble = paddDouble;

  xmlTextReaderPtr reader;
  int ret;

  /* Using vector instead of stack as need to access earlier tags */
  std::vector<std::string> tags;
  /* name of current agent type, "" if invalid */
  std::string agent = "";

  /* Open file to read */
  reader = xmlReaderForFile(file_name.c_str(), NULL, 0);
  /* Check if file opened successfully */
  if (reader == NULL)
    throw std::runtime_error("Unable to open xml pop file");

  // set path to xml pop location
  setXmlPopPath(file_name);
  // validate data using a schema
  validateData(file_name);

#ifndef TESTBUILD
  printf("Reading file: %s\n", file_name.c_str());
#endif

  /* Read the first node */
  ret = xmlTextReaderRead(reader);
  /* Continue reading nodes until end */
  while (ret == 1) {
    /* Process node */
    processNode(reader, &tags, &agent);
    /* Read next node */
    ret = xmlTextReaderRead(reader);
  }
  /* Clean up */
  xmlFreeTextReader(reader);
  /* If error reading node return */
  if (ret != 0)
    throw std::runtime_error("Failed to parse xml pop file");
}

bool IOXMLPop::xmlPopPathIsSet() {
  return xml_pop_path_is_set;
}

std::string IOXMLPop::xmlPopPath() {
  return xml_pop_path;
}

void IOXMLPop::setXmlPopPath(std::string path) {
  /* Set the xml pop path to the directory of the opened file.
   * This path is then used as the root directory to write xml pop to. */
  boost::filesystem::path p(path);
  boost::filesystem::path dir = p.parent_path();
  xml_pop_path = dir.string();
  if (xml_pop_path != "")
    xml_pop_path.append("/");
  xml_pop_path_is_set = true;
}

void IOXMLPop::createDataSchemaHead() {
  // Open root tag
  writer_.writeXMLTag("xs:schema");
  writer_.writeXMLTagAttribute(
      "xmlns:xs", "http://www.w3.org/2001/XMLSchema");
  writer_.writeXMLTagAttribute("elementFormDefault", "qualified");
  writer_.writeXMLTag("xs:annotation");
  writer_.writeXMLTag("xs:documentation", "Auto-generated data schema");
  // Close the element named xs:annotation
  writer_.writeXMLEndTag();
}

void IOXMLPop::createDataSchemaAgentNameType() {
  // Define agent name type
  writer_.writeXMLTag("xs:simpleType");
  writer_.writeXMLTagAttribute("name", "agentType");
  writer_.writeXMLTag("xs:restriction");
  writer_.writeXMLTagAttribute("base", "xs:string");

  AgentMemory::const_iterator agent;

  // for each agent
  for (agent = agentMemory_.begin(); agent != agentMemory_.end(); ++agent) {
    writer_.writeXMLTag("xs:enumeration");
    writer_.writeXMLTagAttribute("value", (*agent).first);
    // Close the element named xs:enumeration
    writer_.writeXMLEndTag();
  }

  // Close the element named xs:restriction
  writer_.writeXMLEndTag();
  // Close the element named xs:simpleType
  writer_.writeXMLEndTag();
}

void IOXMLPop::createDataSchemaAgentVarChoice() {
  // Define agent variables
  writer_.writeXMLTag("xs:group");
  writer_.writeXMLTagAttribute("name", "agent_vars");
  writer_.writeXMLTag("xs:choice");

  AgentMemory::const_iterator agent;

  // for each agent
  for (agent = agentMemory_.begin(); agent != agentMemory_.end(); ++agent) {
    writer_.writeXMLTag("xs:group");
    std::string ref = "agent_";
    ref.append((*agent).first);
    ref.append("_vars");
    writer_.writeXMLTagAttribute("ref", ref);
    // Close the element named xs:group
    writer_.writeXMLEndTag();
  }

  // Close the element named xs:choice
  writer_.writeXMLEndTag();
  // Close the element named xs:group
  writer_.writeXMLEndTag();
}

void IOXMLPop::createDataSchemaAgentVar(std::string type, std::string name) {
  std::string schema_type;
  // Write tag
  writer_.writeXMLTag("xs:element");
  writer_.writeXMLTagAttribute("name", name);
  // Select correct schema data type
  if (type == "int") schema_type = "xs:integer";
  else if (type == "double") schema_type = "xs:double";
  else
    schema_type = "xs:string";
  // Write schema data type attribute
  writer_.writeXMLTagAttribute("type", schema_type);
  // Close the element named xs:element
  writer_.writeXMLEndTag();
}

void IOXMLPop::createDataSchemaAgentVars() {
  AgentMemory::const_iterator agent;
  VarVec::const_iterator var;

  // for each agent
  for (agent = agentMemory_.begin(); agent != agentMemory_.end(); ++agent) {
    std::string name = "agent_";
    name.append((*agent).first);
    name.append("_vars");
    // Create a group element for the agent type
    writer_.writeXMLTag("xs:group");
    writer_.writeXMLTagAttribute("name", name);
    writer_.writeXMLTag("xs:sequence");
    VarVec variables = (*agent).second;
    for (var = variables.begin(); var != variables.end(); ++var)
      createDataSchemaAgentVar((*var).first, (*var).second);
    // Close the element named xs:sequence
    writer_.writeXMLEndTag();
    // Close the element named xs:group
    writer_.writeXMLEndTag();
  }
}

void IOXMLPop::createDataSchemaDefineAgents() {
  // Define agents
  writer_.writeXMLTag("xs:element");
  writer_.writeXMLTagAttribute("name", "xagent");
  writer_.writeXMLTag("xs:annotation");
  writer_.writeXMLTag("xs:documentation", "Agent data");
  // Close the element named xs:annotation
  writer_.writeXMLEndTag();
  writer_.writeXMLTag("xs:complexType");
  writer_.writeXMLTag("xs:sequence");
  writer_.writeXMLTag("xs:element");
  writer_.writeXMLTagAttribute("name", "name");
  writer_.writeXMLTagAttribute("type", "agentType");
  // Close the element named xs:element
  writer_.writeXMLEndTag();
  writer_.writeXMLTag("xs:group");
  writer_.writeXMLTagAttribute("ref", "agent_vars");
  // Close the element named xs:group, xs:sequence,
  // xs:complexType, xs:element
  writer_.writeXMLEndTag(4);
}

void IOXMLPop::createDataSchemaDefineTags() {
  // XML Tag structure
  writer_.writeXMLTag("xs:element");
  writer_.writeXMLTagAttribute("name", "states");
  writer_.writeXMLTag("xs:complexType");
  writer_.writeXMLTag("xs:sequence");
  writer_.writeXMLTag("xs:element");
  writer_.writeXMLTagAttribute("name", "itno");
  writer_.writeXMLTagAttribute("type", "xs:nonNegativeInteger");
  writer_.writeXMLTag("xs:annotation");
  writer_.writeXMLTag("xs:documentation", "Iteration number");
  /* Close the element named xs:annotation, xs:element */
  writer_.writeXMLEndTag(2);
  writer_.writeXMLTag("xs:element");
  writer_.writeXMLTagAttribute("ref", "xagent");
  writer_.writeXMLTagAttribute("minOccurs", "0");
  writer_.writeXMLTagAttribute("maxOccurs", "unbounded");
  /* Close the element named xs:element, xs:sequence,
   * xs:complexType, xs:element */
  writer_.writeXMLEndTag(4);
}

void IOXMLPop::createDataSchema(xmlBufferPtr * buf) {
  // XML text writer
  xmlTextWriterPtr writer;

  // create a new XML buffer
  *buf = xmlBufferCreate();
  if (*buf == NULL)
    throw std::runtime_error("Could not create buffer for XML schema");

  /* Open file to write to, with no compression */
  writer = xmlNewTextWriterMemory(*buf, 0);
  if (writer == NULL)
    throw std::runtime_error("Could not create writer for XML schema");

  // setup xml writer
  writer_.setWriterPtr(writer);

  createDataSchemaHead();
  createDataSchemaAgentNameType();
  createDataSchemaAgentVarChoice();
  createDataSchemaAgentVars();
  createDataSchemaDefineAgents();
  createDataSchemaDefineTags();

  /* End xml file, automatically ends schema tag */
  writer_.endXMLDoc();

  // free XML writer
  xmlFreeTextWriter(writer);
}

void IOXMLPop::openXMLDoc(xmlDocPtr * doc, std::string const& data_file) {
  *doc = xmlReadFile(data_file.c_str(), NULL, 0);
  /* Return error if the file was not successfully parsed */
  if (*doc == NULL) throw std::runtime_error(
      "Parsing of XML file failed");
}

void IOXMLPop::validateData(std::string const& data_file) {
  // pop file doc
  xmlDocPtr doc = NULL;
  // schema buffer
  xmlBufferPtr schema_buf;

  /* Try and open pop data xml */
  openXMLDoc(&doc, data_file);
  /* If successful try and open schema */
  createDataSchema(&schema_buf);

  // create schema parser context pointer
  xmlSchemaParserCtxtPtr parser_ctxt = xmlSchemaNewMemParserCtxt(
      reinterpret_cast<const char *>(xmlBufferContent(schema_buf)),
      xmlBufferLength(schema_buf));
  if (parser_ctxt == NULL) throw std::runtime_error(
      "Unable to create a parser context for pop schema");

  // create schema parser
  xmlSchemaPtr schema = xmlSchemaParse(parser_ctxt);
  if (schema == NULL) throw std::runtime_error(
      "The pop schema is not valid");

  // create schema validation context
  xmlSchemaValidCtxtPtr valid_ctxt = xmlSchemaNewValidCtxt(schema);
  if (valid_ctxt == NULL) throw std::runtime_error(
      "Unable to create a validation context for the pop schema");

  /* 0 if valid, positive error code otherwise
   * -1 in case of internal or API error */
  int rc = xmlSchemaValidateDoc(valid_ctxt, doc);

  /* Free all pointers */
  xmlSchemaFreeValidCtxt(valid_ctxt);
  xmlSchemaFree(schema);
  xmlSchemaFreeParserCtxt(parser_ctxt);
  xmlFreeDoc(doc);
  xmlBufferFree(schema_buf);

  // check error code from validation
  if (rc == -1)
    throw std::runtime_error("Internal error validating pop file");
  else if (rc != 0)
    throw std::runtime_error("Error validating pop file");
}

void IOXMLPop::processStartNode(std::vector<std::string> * tags,
    std::string name, xmlTextReaderPtr reader) {
  /* If correct tag at correct depth with
   * correct tag name */
  if ((tags->size() == 0 && name == "states")
      || (tags->size() == 1
          && (name == "itno" || name == "environment" || name == "xagent"))
      || tags->size() == 2) {
    tags->push_back(name);
  } else {
    xmlFreeTextReader(reader);
    throw std::runtime_error(std::string("Unknown xml tag: ").append(name));
  }
}

void IOXMLPop::processTextVariable(std::string value,
    std::vector<std::string> * tags, std::string * agent) {
  AgentMemory::const_iterator it;
  // get agent memory info of agent
  it = agentMemory_.find(*agent);
  // try and find type of variable from tag name
  std::string var_type = "";
  VarVec::const_iterator var;
  // for each var in var list
  for (var = (*it).second.begin(); var != (*it).second.end(); ++var)
    if ((*var).second == tags->back()) var_type = (*var).first;
  // if variable name does not exist then throw exception
  if (var_type == "") throw std::runtime_error(
      std::string("Agent variable is not recognised: ").append(value));

  // check variable type for casting and use appropriate casting function
  if (var_type == "int") {
    int i;
    try {
      i = boost::lexical_cast<int>(value);
    } catch(const boost::bad_lexical_cast&) { throw std::runtime_error(
      std::string("Variable could not be cast to correct type: ").append(
              value).append(" in ").append(tags->back())); }
    addInt((*agent), tags->back(), i);
  } else if (var_type == "double") {
    double d;
    try {
      d = boost::lexical_cast<double>(value);
    } catch(const boost::bad_lexical_cast&) { throw std::runtime_error(
      std::string("Variable could not be cast to correct type: ").append(
              value).append(" in ").append(tags->back())); }
    addDouble((*agent), tags->back(), d);
  }
}

void IOXMLPop::processTextAgent(std::vector<std::string> * tags,
    xmlTextReaderPtr reader, std::string * agent) {
  /* Read value */
  std::string value = reinterpret_cast<const char*>(xmlTextReaderConstValue(
      reader));
  /* If tag is the agent name */
  if (tags->back() == "name") {
    /* Check if agent is part of this model. If agent name is unknown */
    AgentMemory::const_iterator it;
    it = agentMemory_.find(value);
    if (it == agentMemory_.end()) {
      xmlFreeTextReader(reader);
      throw std::runtime_error(
          std::string("Agent type is not recognised: ").append(value));
    }
    (*agent) = value;
  } else {
    if (*agent != "")  /* Check if agent exists */
      processTextVariable(value, tags, agent);
    else
      throw std::runtime_error(
        "Trying to read agent variables but agent name has not been found");
  }
}

void IOXMLPop::processEndNode(std::vector<std::string> * tags, std::string name,
    std::string * agent) {
  /* Check end tag closes opened tag */
  if (tags->back() == name) {
    /* If end of agent then reset agent pointer */
    if (name == "xagent") *agent = "";
    tags->pop_back();
  } else {
    /* This error is handled by the xml library and
     * should never be reached */
    throw std::runtime_error(
        std::string("Error: Tag is not closed properly: ").
        append(name).
        append(" with ").
        append(tags->back()));
  }
}

void IOXMLPop::processNode(xmlTextReaderPtr reader,
    std::vector<std::string> * tags,
    std::string * agent) {
  /* Node name */
  const xmlChar *xmlname;
  /* Node name as strings */
  std::string name;

  /* Read node name */
  xmlname = xmlTextReaderConstName(reader);
  if (xmlname == NULL) xmlname = BAD_CAST "--";
  /* Convert xmlChar to string */
  name = reinterpret_cast<const char*>(xmlname);

  /* Node types
   * 1  - Start element
   * 3  - Text
   * 14 - White space
   * 15 - End element */

  /* Handle node */
  switch (xmlTextReaderNodeType(reader)) {
  case 1: /* Start element */
    processStartNode(tags, name, reader);
    break;
  case 3: /* Text */
    if (tags->size() == 3 && tags->at(1) == "xagent")
      processTextAgent(tags, reader, agent);
    break;
  case 15: /* End element */
    processEndNode(tags, name, agent);
    break;
  }
}

std::string IOXMLPop::getName() {
  return "xml";
}

// set 'C' linkage for function names
extern "C" {
  // function to return an instance of a new IO plugin object
  IO* construct() {
    return new IOXMLPop();
  }
}

}}  // namespace flame::io
