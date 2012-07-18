/*!
 * \file src/io/io_xml_pop.cpp
 * \author Simon Coakley
 * \date 2012
 * \copyright Copyright (c) 2012 STFC Rutherford Appleton Laboratory
 * \copyright Copyright (c) 2012 University of Sheffield
 * \copyright GNU Lesser General Public License
 * \brief IOXMLPop: reading of population XML file
 */
#include <boost/filesystem.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/variant.hpp>
#include <libxml/xmlreader.h>
#include <libxml/xmlwriter.h>
#include <string>
#include <vector>
#include <cstdio>
#include "./io_xml_pop.hpp"

namespace model = flame::model;

namespace flame { namespace io { namespace xml {

IOXMLPop::IOXMLPop() {
    xml_pop_path_is_set = false;
}

template <class T>
int IOXMLPop::setupVectorReader(model::XMachine * agent,
        model::XVariable * var,
        flame::mem::MemoryManager * memoryManager, size_t * noAgents,
        std::vector< boost::variant<intVecPtr, doubleVecPtr> > * varVectors,
        size_t jj) {
    /* Create vector reader.. */
    std::vector<T> * ro = memoryManager->GetVector<T>(
                    agent->getName(), var->getName());
    /* ..and add to list of vectors. */
    varVectors->push_back(ro);
    /* Check array length */
    if (jj == 0) *noAgents = ro->size();
    else if (ro->size() != *noAgents) {
fprintf(stderr, "Error: Memory vector size does not correspond: '%s'\n",
            var->getName().c_str());
        return 3;
    }

    return 0;
}

int IOXMLPop::setupVectorReaders(model::XMachine * agent,
        flame::mem::MemoryManager * memoryManager, size_t * noAgents,
        std::vector< boost::variant<intVecPtr, doubleVecPtr> > * varVectors) {
    size_t jj;

    /* For each memory variable */
    for (jj = 0; jj < agent->getVariables()->size(); jj++) {
        /* Assign to local xvariable variable */
        model::XVariable * var = agent->getVariables()->at(jj);

        /* Set up vector readers for each memory variable
         * dependent on variable data type and add to a
         * list ready to be used to write each agents
         * memory out in one go.
         */
        if (var->getType() == "int")
            setupVectorReader<int>(
                    agent, var, memoryManager, noAgents, varVectors, jj);
        if (var->getType() == "double")
            setupVectorReader<double>(
                    agent, var, memoryManager, noAgents, varVectors, jj);
    }

    return 0;
}

int IOXMLPop::writeXMLAgentVariables(model::XMachine * agent,
        xmlTextWriterPtr writer,
        std::vector< boost::variant<intVecPtr, doubleVecPtr> > * varVectors,
        size_t kk) {
    size_t jj;
    int rc;

    /* For each memory variable */
    for (jj = 0;
        jj < agent->getVariables()->size(); jj++) {
        /* Assign to local xvariable variable */
        model::XVariable * var = agent->getVariables()->at(jj);

        /* Write variable value dependent on the variable
         * type by accessing the associated vector reader
         * from the list of vector readers and taking the
         * nth element corresponding with the nth agent.
         */
        if (var->getType() == "int") {
            rc = writeXMLTag(writer, var->getName(),
                    *(boost::get<intVecPtr>(
                    varVectors->at(jj))->begin()+kk));
            if (rc != 0) return rc;
        }
        if (var->getType() == "double") {
            rc = writeXMLTag(writer, var->getName(),
                *(boost::get<doubleVecPtr>(
                varVectors->at(jj))->begin()+kk) );
            if (rc != 0) return rc;
        }
    }

    return 0;
}

int IOXMLPop::writeXMLAgent(model::XMachine * agent,
        flame::mem::MemoryManager * memoryManager, xmlTextWriterPtr writer) {
    int rc;
    size_t kk;
    /* List of memory vector readers populated for each agent */
    std::vector< boost::variant<intVecPtr, doubleVecPtr> > varVectors;
    /* The number of agents per agent type */
    size_t noAgents;

    /* Setup vector readers for agent memory variables */
    rc = setupVectorReaders(agent, memoryManager, &noAgents, &varVectors);
    if (rc != 0) return rc;

    /* For each agent in the simulation */
    for (kk = 0; kk < noAgents; kk++) {
        /* Open root tag */
        rc = writeXMLTag(writer, "xagent");
        if (rc != 0) return rc;

        /* Write agent name */
        rc = writeXMLTag(writer, "name", agent->getName());
        if (rc != 0) return rc;

        /* Write agent variables */
        rc = writeXMLAgentVariables(agent, writer, &varVectors, kk);
        if (rc != 0) return rc;

        /* Close the element named xagent. */
        writeXMLEndTag(writer);
    }

    /* Clear the memory vector reader list for the next agent type */
    varVectors.clear();

    return 0;
}

int IOXMLPop::writeXMLPop(std::string file_name,
        int iterationNo, model::XModel * model,
        flame::mem::MemoryManager * memoryManager) {
    /* Return code */
    int rc = 0;
    /* The xml text writer */
    xmlTextWriterPtr writer;
    /* Loop variables */
    size_t ii;

    printf("Writing file: %s\n", file_name.c_str());

    /* Open file to write to, with no compression */
    writer = xmlNewTextWriterFilename(file_name.c_str(), 0);
    if (writer == NULL) {
        fprintf(stderr, "Error: Opening xml population file to write to\n");
        return 1;
    }
    /* Write tags on new lines */
    xmlTextWriterSetIndent(writer, 1);

    /* Open root tag */
    rc = writeXMLTag(writer, "states");
    if (rc != 0) return rc;

    /* Write itno tag with iteration number */
    rc = writeXMLTag(writer, "itno", iterationNo);
    if (rc != 0) return rc;

    /* For each agent type in the model */
    for (ii = 0; ii < model->getAgents()->size(); ii++) {
        rc = writeXMLAgent(model->getAgents()->at(ii), memoryManager, writer);
        if (rc != 0) return rc;
    }

    /* End xml file, automatically ends states tag */
    rc = endXMLDoc(writer);

    /* Free the xml writer */
    xmlFreeTextWriter(writer);

    return rc;
}

int IOXMLPop::readXMLPop(std::string file_name, model::XModel * model,
        flame::mem::MemoryManager * memoryManager) {
    xmlTextReaderPtr reader;
    int ret, rc = 0;
    /* Using vector instead of stack as need to access earlier tags */
    std::vector<std::string> tags;
    /* Pointer to current agent type, 0 if invalid */
    model::XMachine * agent = 0;

    /* Open file to read */
    reader = xmlReaderForFile(file_name.c_str(), NULL, 0);
    /* Check if file opened successfully */
    if (reader == NULL) {
        fprintf(stderr, "Error: Unable to open: '%s'\n", file_name.c_str());
        return 1;
    }

    /* Read the first node */
    ret = xmlTextReaderRead(reader);
    /* Continue reading nodes until end */
    while (ret == 1 && rc == 0) {
        /* Process node */
        rc = processNode(reader, model, memoryManager, &tags, &agent);
        /* Read next node */
        ret = xmlTextReaderRead(reader);
    }
    /* Clean up */
    xmlFreeTextReader(reader);
    /* If return code error */
    if (rc != 0) return rc;
    /* If error reading node return */
    if (ret != 0) {
        fprintf(stderr, "Error: Failed to parse: '%s'\n",
            file_name.c_str());
        return 2;
    }

    /* Return successfully */
    return 0;
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
    xml_pop_path.append("/");
    xml_pop_path_is_set = true;
}

int IOXMLPop::createDataSchemaHead(xmlTextWriterPtr writer) {
    int rc = 0;
    /* Open root tag */
    rc = writeXMLTag(writer, "xs:schema");
    if (rc == 0) rc = writeXMLTagAttribute(writer, "xmlns:xs",
            "http://www.w3.org/2001/XMLSchema");
    if (rc == 0) rc = writeXMLTagAttribute(writer,
            "elementFormDefault", "qualified");
    if (rc == 0) rc = writeXMLTag(writer, "xs:annotation");
    if (rc == 0) rc = writeXMLTag(writer, "xs:documentation",
            "Auto-generated data schema");
    /* Close the element named xs:annotation. */
    if (rc == 0) rc = writeXMLEndTag(writer);

    return rc;
}

int IOXMLPop::createDataSchemaAgentNameType(xmlTextWriterPtr writer,
        flame::model::XModel * model) {
    int rc = 0;
    std::vector<model::XMachine*>::iterator agent;
    /* Define agent name type */
    if (rc == 0) rc = writeXMLTagAndAttribute(writer, "xs:simpleType",
            "name", "agentType");
    if (rc == 0) rc = writeXMLTagAndAttribute(writer, "xs:restriction",
            "base", "xs:string");

    for (agent = model->getAgents()->begin();
            agent != model->getAgents()->end();
            agent++) {
        if (rc == 0) rc = writeXMLTag(writer, "xs:enumeration");
        if (rc == 0) rc = writeXMLTagAttribute(writer, "value",
                (*agent)->getName());
        /* Close the element named xs:enumeration. */
        if (rc == 0) rc = writeXMLEndTag(writer);
    }

    /* Close the element named xs:restriction. */
    if (rc == 0) rc = writeXMLEndTag(writer);
    /* Close the element named xs:simpleType. */
    if (rc == 0) rc = writeXMLEndTag(writer);
    return rc;
}

int IOXMLPop::createDataSchemaAgentVarChoice(xmlTextWriterPtr writer,
        flame::model::XModel * model) {
    int rc = 0;
    std::vector<model::XMachine*>::iterator agent;
    /* Define agent variables */
    if (rc == 0) rc = writeXMLTagAndAttribute(writer, "xs:group",
            "name", "agent_vars");
    if (rc == 0) rc = writeXMLTag(writer, "xs:choice");

    for (agent = model->getAgents()->begin();
            agent != model->getAgents()->end();
            agent++) {
        rc = writeXMLTag(writer, "xs:group");
        if (rc != 0) return rc;
        std::string ref = "agent_";
        ref.append((*agent)->getName());
        ref.append("_vars");
        rc = writeXMLTagAttribute(writer, "ref", ref);
        if (rc != 0) return rc;
        /* Close the element named xs:group. */
        writeXMLEndTag(writer);
    }

    /* Close the element named xs:choice. */
    if (rc == 0) rc = writeXMLEndTag(writer);
    /* Close the element named xs:group. */
    if (rc == 0) rc = writeXMLEndTag(writer);
    return rc;
}

int IOXMLPop::createDataSchemaAgentVar(xmlTextWriterPtr writer,
        std::vector<model::XVariable*>::iterator variable) {
    int rc;
    std::string type;
    /* Write tag */
    rc = writeXMLTagAndAttribute(writer, "xs:element", "name",
            (*variable)->getName());
    if (rc != 0) return rc;
    /* Select correct schema data type */
    if ((*variable)->getType() == "int") type = "xs:integer";
    else if ((*variable)->getType() == "double") type = "xs:double";
    else
        type = "xs:string";
    /* Write schema data type attribute */
    rc = writeXMLTagAttribute(writer, "type", type);
    if (rc != 0) return rc;
    /* Close the element named xs:element. */
    rc = writeXMLEndTag(writer);
    return rc;
}

int IOXMLPop::createDataSchemaAgentVars(xmlTextWriterPtr writer,
        flame::model::XModel * model) {
    int rc = 0;
    std::vector<model::XMachine*>::iterator agent;
    std::vector<model::XVariable*>::iterator variable;
    /* For each agent type */
    for (agent = model->getAgents()->begin();
            agent != model->getAgents()->end(); agent++) {
        std::string name = "agent_";
        name.append((*agent)->getName());
        name.append("_vars");
        /* Create a group element for the agent type */
        rc = writeXMLTagAndAttribute(writer, "xs:group", "name", name);
        if (rc != 0) return rc;
        rc = writeXMLTag(writer, "xs:sequence");
        if (rc != 0) return rc;
        for (variable = (*agent)->getVariables()->begin();
                variable != (*agent)->getVariables()->end(); variable++) {
            rc = createDataSchemaAgentVar(writer, variable);
            if (rc != 0) return rc;
        }
        /* Close the element named xs:sequence. */
        writeXMLEndTag(writer);
        /* Close the element named xs:group. */
        writeXMLEndTag(writer);
    }
    return rc;
}

int IOXMLPop::createDataSchemaDefineAgents(xmlTextWriterPtr writer) {
    int rc = 0;
    /* Define agents */
    rc = writeXMLTagAndAttribute(writer, "xs:element", "name", "xagent");
    if (rc == 0) rc = writeXMLTag(writer, "xs:annotation");
    if (rc == 0) rc = writeXMLTag(writer, "xs:documentation", "Agent data");
    /* Close the element named xs:annotation. */
    if (rc == 0) rc = writeXMLEndTag(writer);
    if (rc == 0) rc = writeXMLTag(writer, "xs:complexType");
    if (rc == 0) rc = writeXMLTag(writer, "xs:sequence");
    if (rc == 0) rc = writeXMLTagAndAttribute(writer,
            "xs:element", "name", "name", "type", "agentType");
    /* Close the element named xs:element. */
    if (rc == 0) rc = writeXMLEndTag(writer);
    if (rc == 0) rc = writeXMLTagAndAttribute(writer, "xs:group",
            "ref", "agent_vars");
    /* Close the element named xs:group, xs:sequence,
     * xs:complexType, xs:element */
    if (rc == 0) rc = writeXMLEndTag(writer, 4);
    return rc;
}

int IOXMLPop::createDataSchemaDefineTags(xmlTextWriterPtr writer) {
    int rc = 0;
    /* XML Tag structure */
    if (rc == 0) rc = writeXMLTagAndAttribute(writer, "xs:element",
            "name", "states");
    if (rc == 0) rc = writeXMLTag(writer, "xs:complexType");
    if (rc == 0) rc = writeXMLTag(writer, "xs:sequence");
    if (rc == 0) rc = writeXMLTagAndAttribute(writer, "xs:element",
            "name", "itno", "type", "xs:nonNegativeInteger");
    if (rc == 0) rc = writeXMLTag(writer, "xs:annotation");
    if (rc == 0) rc = writeXMLTag(writer, "xs:documentation",
            "Iteration number");
    /* Close the element named xs:annotation, xs:element */
    if (rc == 0) rc = writeXMLEndTag(writer, 2);
    if (rc == 0) rc = writeXMLTagAndAttribute(writer,
            "xs:element", "ref", "xagent", "minOccurs", "0",
            "maxOccurs", "unbounded");
    /* Close the element named xs:element, xs:sequence,
     * xs:complexType, xs:element */
    if (rc == 0) rc = writeXMLEndTag(writer, 4);
    return rc;
}

int IOXMLPop::createDataSchema(std::string const& file,
        flame::model::XModel * model) {
    /* Return code */
    int rc = 0;
    std::vector<model::XMachine*>::iterator agent;
    std::vector<model::XVariable*>::iterator variable;
    /* The xml text writer */
    xmlTextWriterPtr writer;

    printf("Writing file: %s\n", file.c_str());

    /* Open file to write to, with no compression */
    writer = xmlNewTextWriterFilename(file.c_str(), 0);
    if (writer == NULL) {
        fprintf(stderr, "Error: Opening data schema file to write to\n");
        return 1;
    }
    /* Write tags on new lines */
    xmlTextWriterSetIndent(writer, 1);
    if (rc == 0) rc = createDataSchemaHead(writer);
    if (rc == 0) rc = createDataSchemaAgentNameType(writer, model);
    if (rc == 0) rc = createDataSchemaAgentVarChoice(writer, model);
    if (rc == 0) rc = createDataSchemaAgentVars(writer, model);
    if (rc == 0) rc = createDataSchemaDefineAgents(writer);
    if (rc == 0) rc = createDataSchemaDefineTags(writer);
    /* End xml file, automatically ends schema tag */
    if (rc == 0) rc = endXMLDoc(writer);

    /* Free the xml writer */
    xmlFreeTextWriter(writer);

    return rc;
}

int IOXMLPop::openXMLDoc(xmlDocPtr * doc, std::string const& data_file) {
    *doc = xmlReadFile(data_file.c_str(), NULL, 0);
    /* Return error if the file was not successfully parsed */
    if (*doc == NULL) {
        std::fprintf(stderr,
                "Error: Data file cannot be opened/parsed: %s\n",
                data_file.c_str());
        return -5;
    }

    return 0;
}

int IOXMLPop::openXMLSchema(xmlSchemaValidCtxtPtr * valid_ctxt,
        std::string const& schema_file,
        xmlSchemaParserCtxtPtr * parser_ctxt, xmlSchemaPtr * schema,
        xmlDocPtr * schema_doc) {
    *schema_doc = xmlReadFile(
                schema_file.c_str(), NULL, XML_PARSE_NONET);
    /* the schema cannot be loaded or is not well-formed */
    if (*schema_doc == NULL) return -1;

    *parser_ctxt = xmlSchemaNewDocParserCtxt(*schema_doc);
    /* unable to create a parser context for the schema */
    if (*parser_ctxt == NULL) return -2;

    *schema = xmlSchemaParse(*parser_ctxt);
    /* the schema itself is not valid */
    if (schema == NULL) return -3;

    *valid_ctxt = xmlSchemaNewValidCtxt(*schema);
    /* unable to create a validation context for the schema */
    if (*valid_ctxt == NULL) return -4;

    return 0;
}

int IOXMLPop::validateData(std::string const& data_file,
        std::string const& schema_file) {
    int rc = 0;
    xmlDocPtr doc = NULL;
    xmlSchemaValidCtxtPtr valid_ctxt = NULL;
    xmlSchemaParserCtxtPtr parser_ctxt = NULL;
    xmlSchemaPtr schema = NULL;
    xmlDocPtr schema_doc = NULL;

    /* Try and open pop data xml */
    rc = openXMLDoc(&doc, data_file);
    /* If successful try and open schema */
    if (rc == 0)
        rc = openXMLSchema(&valid_ctxt, schema_file,
                &parser_ctxt, &schema, &schema_doc);

    /* 0 if valid, positive error code otherwise
     * -1 in case of internal or API error */
    if (rc == 0)
        rc = xmlSchemaValidateDoc(valid_ctxt, doc);

    /* Free all pointers */
    xmlSchemaFreeValidCtxt(valid_ctxt);
    xmlSchemaFree(schema);
    xmlSchemaFreeParserCtxt(parser_ctxt);
    xmlFreeDoc(schema_doc);
    xmlFreeDoc(doc);

    return rc;
}

int IOXMLPop::processStartNode(std::vector<std::string> * tags,
        std::string name) {
    /* If correct tag at correct depth with
     * correct tag name */
    if ((tags->size() == 0 && name == "states") ||
        (tags->size() == 1 &&
        (name == "itno" || name == "environment" || name == "xagent")) ||
        tags->size() == 2) {
        tags->push_back(name);
    } else {
        fprintf(stderr, "Error: Unknown tag: '%s'\n",
                name.c_str());
        return 3;
    }
    return 0;
}

template <class T>
int IOXMLPop::processTextVariableCast(std::string value,
        std::vector<std::string> * tags,
        flame::mem::MemoryManager * memoryManager, model::XMachine ** agent) {
    T typeValue;
    try {
        typeValue = boost::lexical_cast<T>(value);
    } catch(const boost::bad_lexical_cast&) {
        std::fprintf(stderr,
"Error: variable could not be cast to correct type: '%s' in '%s'\n",
            value.c_str(),
            tags->back().c_str());
        return 6;
    }
    /* Add value to memory manager */
    std::vector<T>* vec =
        memoryManager->GetVector<T>(
            (*agent)->getName(), tags->back());
    vec->push_back(typeValue);
    return 0;
}

int IOXMLPop::processTextVariable(std::string value,
        std::vector<std::string> * tags,
        flame::mem::MemoryManager * memoryManager, model::XMachine ** agent) {
    int rc;
    /* Get pointer to variable type */
    model::XVariable * var = (*agent)->getVariable(tags->back());
    /* Check if variable is part of the agent */
    if (var) {
        /* Check variable type for casting and
         * use appropriate casting function */
        if (var->getType() == "int") {
            rc = processTextVariableCast<int>(
                    value, tags, memoryManager, agent);
            if (rc != 0) return rc;
        } else if (var->getType() == "double") {
            rc = processTextVariableCast<double>(
                    value, tags, memoryManager, agent);
            if (rc != 0) return rc;
        }
    } else {
        fprintf(stderr, "Error: Agent variable is not recognised: '%s'\n",
                tags->back().c_str());
        return 5;
    }

    return 0;
}

int IOXMLPop::processTextAgent(std::vector<std::string> * tags,
        xmlTextReaderPtr reader,
        model::XMachine ** agent, model::XModel * model,
        flame::mem::MemoryManager * memoryManager) {
    int rc = 0;

    /* Read value */
    std::string value = reinterpret_cast<const char*>
            (xmlTextReaderConstValue(reader));
    /* If tag is the agent name */
    if (tags->back() == "name") {
        /* Check if agent is part of this model */
        (*agent) = model->getAgent(value);
        /* If agent name is unknown */
        if (!(*agent)) {
fprintf(stderr, "Error: Agent type is not recognised: '%s'\n", value.c_str());
            rc = 4;
        }
    } else { if (*agent) /* Check if agent exists */
        rc = processTextVariable(value, tags, memoryManager, agent);
    }

    return rc;
}

int IOXMLPop::processEndNode(std::vector<std::string> * tags, std::string name,
        model::XMachine ** agent) {
    /* Check end tag closes opened tag */
    if (tags->back() == name) {
        /* If end of agent then reset agent pointer */
        if (name == "xagent") (*agent) = 0;
        tags->pop_back();
    } else {
        /* This error is handled by the xml library and
         * should never be reached */
fprintf(stderr, "Error: Tag is not closed properly: '%s' with '%s'\n",
                name.c_str(),
                tags->back().c_str());
        return 9;
    }
    return 0;
}

int IOXMLPop::processNode(xmlTextReaderPtr reader, model::XModel * model,
        flame::mem::MemoryManager * memoryManager,
        std::vector<std::string> * tags, model::XMachine ** agent) {
    int rc = 0;
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
            rc = processStartNode(tags, name);
            break;
        case 3: /* Text */
            if (tags->size() == 3 && tags->at(1) == "xagent")
                rc = processTextAgent(
                    tags, reader, agent, model, memoryManager);
            break;
        case 15: /* End element */
            rc = processEndNode(tags, name, agent);
            break;
    }
    if (rc != 0) return rc;

    return 0;
}

int IOXMLPop::writeXMLEndTag(xmlTextWriterPtr writer) {
    int rc;
    rc = xmlTextWriterEndElement(writer);
    if (rc < 0) {
        fprintf(stderr, "Error: Writing to xml file\n");
        return 2;
    }
    return 0;
}

int IOXMLPop::writeXMLEndTag(xmlTextWriterPtr writer, int count) {
    int rc, ii;
    for (ii = 0; ii < count; ii++) {
        rc = writeXMLEndTag(writer);
        if (rc != 0) return rc;
    }
    return 0;
}

int IOXMLPop::writeXMLTag(xmlTextWriterPtr writer, std::string name) {
    int rc;
    rc = xmlTextWriterStartElement(writer, BAD_CAST name.c_str());
    if (rc < 0) {
        fprintf(stderr, "Error: Writing to xml file\n");
        return 2;
    }
    return 0;
}

int IOXMLPop::writeXMLTag(xmlTextWriterPtr writer,
        std::string name, int value) {
    int rc;
    rc = xmlTextWriterWriteFormatElement(writer, BAD_CAST name.c_str(),
                                                 "%d", value);
    if (rc < 0) {
        fprintf(stderr, "Error: Writing to xml file\n");
        return 2;
    }
    return 0;
}

int IOXMLPop::writeXMLTag(xmlTextWriterPtr writer,
        std::string name, double value) {
    int rc;
    rc = xmlTextWriterWriteFormatElement(writer, BAD_CAST name.c_str(),
                                                 "%f", value);
    if (rc < 0) {
        fprintf(stderr, "Error: Writing to xml file\n");
        return 2;
    }
    return 0;
}

int IOXMLPop::writeXMLTag(xmlTextWriterPtr writer, std::string name,
        std::string value) {
    int rc;
    rc = xmlTextWriterWriteFormatElement(writer, BAD_CAST name.c_str(),
                                                 "%s", value.c_str());
    if (rc < 0) {
        fprintf(stderr, "Error: Writing to xml file\n");
        return 2;
    }
    return 0;
}

int IOXMLPop::writeXMLTagAttribute(xmlTextWriterPtr writer,
        std::string name, std::string value) {
    int rc;
    rc = xmlTextWriterWriteAttribute(writer,
            BAD_CAST name.c_str(), BAD_CAST value.c_str());
    if (rc < 0) {
        fprintf(stderr, "Error: Writing to xml file\n");
        return 2;
    }
    return 0;
}

int IOXMLPop::writeXMLTagAndAttribute(xmlTextWriterPtr writer, std::string tag,
        std::string name, std::string value) {
    int rc;
    /* Write tag with 1 attribute */
    rc = writeXMLTag(writer, tag);
    if (rc != 0) return rc;
    rc = writeXMLTagAttribute(writer, name, value);
    return rc;
}

int IOXMLPop::writeXMLTagAndAttribute(xmlTextWriterPtr writer, std::string tag,
        std::string name1, std::string value1,
        std::string name2, std::string value2) {
    int rc;
    /* Write tag with 2 attributes */
    rc = writeXMLTag(writer, tag);
    if (rc != 0) return rc;
    rc = writeXMLTagAttribute(writer, name1, value1);
    if (rc != 0) return rc;
    rc = writeXMLTagAttribute(writer, name2, value2);
    return rc;
}

int IOXMLPop::writeXMLTagAndAttribute(xmlTextWriterPtr writer, std::string tag,
        std::string name1, std::string value1,
        std::string name2, std::string value2,
        std::string name3, std::string value3) {
    int rc;
    /* Write tag with
     * 3 attributes */
    rc = writeXMLTag(writer, tag);
    if (rc != 0) return rc;
    rc = writeXMLTagAttribute(writer, name1, value1);
    if (rc != 0) return rc;
    rc = writeXMLTagAttribute(writer, name2, value2);
    if (rc != 0) return rc;
    rc = writeXMLTagAttribute(writer, name3, value3);
    return rc;
}

int IOXMLPop::endXMLDoc(xmlTextWriterPtr writer) {
    int rc;
    rc = xmlTextWriterEndDocument(writer);
    if (rc < 0) {
        fprintf(stderr, "Error: Writing to xml file\n");
        return 2;
    }
    return 0;
}

}}}  // namespace flame::io::xml
