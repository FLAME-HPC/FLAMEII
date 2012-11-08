/*!
 * \file flame2/io/io_xml_pop.cpp
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
#include <utility>
#include "flame2/config.hpp"
#include "flame2/mem/vector_wrapper.hpp"
#include "flame2/exceptions/io.hpp"
#include "io_xml_pop.hpp"

namespace model = flame::model;
namespace exc = flame::exceptions;

namespace flame { namespace io { namespace xml {

IOXMLPop::IOXMLPop() : iteration_(0), xml_pop_path_is_set(false) {}

// This method is empty because you can't (without a lot of difficulty)
// write xml row-wise
void IOXMLPop::writePop(std::string agent_name, std::string var_name) {}

void IOXMLPop::initialiseData() {
    // Write out xml start and environment data when inplemented
}

struct VarVecData {
    VarVecData(std::string s, void* ptr, flame::mem::VectorWrapperBase* v) :
        varName(s), p(ptr), vw(v) {}
    std::string varName;
    void* p;
    flame::mem::VectorWrapperBase* vw;
};

void IOXMLPop::writeAgents(xmlTextWriterPtr writer) {
    std::vector<std::string>::iterator sit;

    // For each agent type in the model
    agentVarMap::iterator it;
    for (it = agentVarMap_.begin(); it != agentVarMap_.end(); ++it) {
        // For each agent variable save name, pointer to data and
        // pointer to vector wrapper
        std::vector<VarVecData> dataMap;
        std::vector<VarVecData>::iterator dit;
        bool stillData = true;
        for (sit = (*it).second.begin(); sit != (*it).second.end(); ++sit) {
            flame::mem::VectorWrapperBase* vw =
flame::mem::MemoryManager::GetInstance().GetVectorWrapper((*it).first, (*sit));
            dataMap.push_back(VarVecData((*sit), vw->GetRawPtr(), vw));
            if (vw->GetRawPtr() == NULL) stillData = false;
        }

        // While there is still data write out each agent to xml
        while (stillData) {
            /* Open root tag */
            writeXMLTag(writer, "xagent");
            /* Write agent name */
            writeXMLTag(writer, "name", (*it).first);
            for (dit = dataMap.begin(); dit != dataMap.end(); ++dit) {
                if (strcmp((*dit).vw->GetDataType()->name(), "i") == 0)
    writeXMLTag(writer, (*dit).varName, *reinterpret_cast<int*>((*dit).p));
                if (strcmp((*dit).vw->GetDataType()->name(), "d") == 0)
    writeXMLTag(writer, (*dit).varName, *reinterpret_cast<double*>((*dit).p));
                (*dit).p = (*dit).vw->StepRawPtr((*dit).p);
            if ((*dit).p == NULL && dit+1 == dataMap.end()) stillData = false;
            }
            /* Close the element named xagent. */
            writeXMLEndTag(writer);
        }
    }
}

void IOXMLPop::finaliseData() {
    // Write out agent data and xml finish
    /* The xml text writer */
    xmlTextWriterPtr writer;

    /* Check a path has been set */
    if (!xmlPopPathIsSet()) {
        throw exc::flame_io_exception(
                "Path not set");
    }

    std::string file_name = xml_pop_path;
    file_name.append(boost::lexical_cast<std::string>(iteration_));
    file_name.append(".xml");

#ifndef TESTBUILD
    printf("Writing file: '%s'\n", file_name.c_str());
#endif

    /* Open file to write to, with no compression */
    writer = xmlNewTextWriterFilename(file_name.c_str(), 0);
    if (writer == NULL)
    throw exc::flame_io_exception("Opening xml population file to write to");
    /* Write tags on new lines */
    xmlTextWriterSetIndent(writer, 1);

    /* Open root tag */
    writeXMLTag(writer, "states");
    // if (rc != 0) return rc;

    /* Write itno tag with iteration number */
    writeXMLTag(writer, "itno", static_cast<int>(iteration_));
    // if (rc != 0) return rc;

    // Write agent memory out
    writeAgents(writer);

    /* End xml file, automatically ends states tag */
    endXMLDoc(writer);

    /* Free the xml writer */
    xmlFreeTextWriter(writer);
}

void IOXMLPop::saveAgentVariableData(model::XModel * model) {
    agentVarMap_.clear();
    boost::ptr_vector<model::XMachine>::iterator agent_it;
    boost::ptr_vector<model::XVariable>::iterator var_it;
    std::pair<agentVarMap::iterator, bool> avm;
    for (agent_it = model->getAgents()->begin();
            agent_it != model->getAgents()->end(); ++agent_it) {
        // Add agent to agent var map for use when writing
        avm = agentVarMap_.insert(std::make_pair((*agent_it).getName(),
                std::vector<std::string>()));
        for (var_it = (*agent_it).getVariables()->begin();
                var_it != (*agent_it).getVariables()->end(); ++var_it) {
            (*avm.first).second.push_back((*var_it).getName());
        }
    }
}

void IOXMLPop::readPop(std::string file_name, model::XModel * model) {
    xmlTextReaderPtr reader;
    int ret;
    /* Using vector instead of stack as need to access earlier tags */
    std::vector<std::string> tags;
    /* Pointer to current agent type, 0 if invalid */
    model::XMachine * agent = 0;

    /* Open file to read */
    reader = xmlReaderForFile(file_name.c_str(), NULL, 0);
    /* Check if file opened successfully */
    if (reader == NULL)
        throw exc::inaccessable_file("Unable to open xml pop file");

#ifndef TESTBUILD
    printf("Reading file: '%s'\n", file_name.c_str());
#endif

    /* Read the first node */
    ret = xmlTextReaderRead(reader);
    /* Continue reading nodes until end */
    while (ret == 1) {
        /* Process node */
        processNode(reader, model, &tags, &agent);
        /* Read next node */
        ret = xmlTextReaderRead(reader);
    }
    /* Clean up */
    xmlFreeTextReader(reader);
    /* If error reading node return */
    if (ret != 0)
        throw exc::unparseable_file("Failed to parse xml pop file");

    /* Save agent vars to a structure */
    saveAgentVariableData(model);
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

void IOXMLPop::createDataSchemaHead(xmlTextWriterPtr writer) {
    // Open root tag
    writeXMLTag(writer, "xs:schema");
    writeXMLTagAttribute(writer, "xmlns:xs",
            "http://www.w3.org/2001/XMLSchema");
    writeXMLTagAttribute(writer,
            "elementFormDefault", "qualified");
    writeXMLTag(writer, "xs:annotation");
    writeXMLTag(writer, "xs:documentation",
            "Auto-generated data schema");
    // Close the element named xs:annotation
    writeXMLEndTag(writer);
}

void IOXMLPop::createDataSchemaAgentNameType(xmlTextWriterPtr writer,
        flame::model::XModel * model) {
    boost::ptr_vector<model::XMachine>::iterator agent;
    // Define agent name type
    writeXMLTagAndAttribute(writer, "xs:simpleType",
            "name", "agentType");
    writeXMLTagAndAttribute(writer, "xs:restriction",
            "base", "xs:string");

    for (agent = model->getAgents()->begin();
            agent != model->getAgents()->end();
            ++agent) {
        writeXMLTag(writer, "xs:enumeration");
        writeXMLTagAttribute(writer, "value",
                (*agent).getName());
        // Close the element named xs:enumeration
        writeXMLEndTag(writer);
    }

    // Close the element named xs:restriction
    writeXMLEndTag(writer);
    // Close the element named xs:simpleType
    writeXMLEndTag(writer);
}

void IOXMLPop::createDataSchemaAgentVarChoice(xmlTextWriterPtr writer,
        flame::model::XModel * model) {
    boost::ptr_vector<model::XMachine>::iterator agent;
    // Define agent variables
    writeXMLTagAndAttribute(writer, "xs:group",
            "name", "agent_vars");
    writeXMLTag(writer, "xs:choice");

    for (agent = model->getAgents()->begin();
            agent != model->getAgents()->end();
            ++agent) {
        writeXMLTag(writer, "xs:group");
        std::string ref = "agent_";
        ref.append((*agent).getName());
        ref.append("_vars");
        writeXMLTagAttribute(writer, "ref", ref);
        // Close the element named xs:group
        writeXMLEndTag(writer);
    }

    // Close the element named xs:choice
    writeXMLEndTag(writer);
    // Close the element named xs:group
    writeXMLEndTag(writer);
}

void IOXMLPop::createDataSchemaAgentVar(xmlTextWriterPtr writer,
        boost::ptr_vector<model::XVariable>::iterator variable) {
    std::string type;
    // Write tag
    writeXMLTagAndAttribute(writer, "xs:element", "name",
            (*variable).getName());
    // Select correct schema data type
    if ((*variable).getType() == "int") type = "xs:integer";
    else if ((*variable).getType() == "double") type = "xs:double";
    else
        type = "xs:string";
    // Write schema data type attribute
    writeXMLTagAttribute(writer, "type", type);
    // Close the element named xs:element
    writeXMLEndTag(writer);
}

void IOXMLPop::createDataSchemaAgentVars(xmlTextWriterPtr writer,
        flame::model::XModel * model) {
    boost::ptr_vector<model::XMachine>::iterator agent;
    boost::ptr_vector<model::XVariable>::iterator variable;
    // For each agent type
    for (agent = model->getAgents()->begin();
            agent != model->getAgents()->end(); ++agent) {
        std::string name = "agent_";
        name.append((*agent).getName());
        name.append("_vars");
        // Create a group element for the agent type
        writeXMLTagAndAttribute(writer, "xs:group", "name", name);
        writeXMLTag(writer, "xs:sequence");
        for (variable = (*agent).getVariables()->begin();
                variable != (*agent).getVariables()->end(); ++variable)
            createDataSchemaAgentVar(writer, variable);
        // Close the element named xs:sequence
        writeXMLEndTag(writer);
        // Close the element named xs:group
        writeXMLEndTag(writer);
    }
}

void IOXMLPop::createDataSchemaDefineAgents(xmlTextWriterPtr writer) {
    // Define agents
    writeXMLTagAndAttribute(writer, "xs:element", "name", "xagent");
    writeXMLTag(writer, "xs:annotation");
    writeXMLTag(writer, "xs:documentation", "Agent data");
    // Close the element named xs:annotation
    writeXMLEndTag(writer);
    writeXMLTag(writer, "xs:complexType");
    writeXMLTag(writer, "xs:sequence");
    writeXMLTagAndAttribute(writer,
            "xs:element", "name", "name", "type", "agentType");
    // Close the element named xs:element
    writeXMLEndTag(writer);
    writeXMLTagAndAttribute(writer, "xs:group",
            "ref", "agent_vars");
    // Close the element named xs:group, xs:sequence,
    // xs:complexType, xs:element
    writeXMLEndTag(writer, 4);
}

void IOXMLPop::createDataSchemaDefineTags(xmlTextWriterPtr writer) {
    // XML Tag structure
    writeXMLTagAndAttribute(writer, "xs:element",
            "name", "states");
    writeXMLTag(writer, "xs:complexType");
    writeXMLTag(writer, "xs:sequence");
    writeXMLTagAndAttribute(writer, "xs:element",
            "name", "itno", "type", "xs:nonNegativeInteger");
    writeXMLTag(writer, "xs:annotation");
    writeXMLTag(writer, "xs:documentation",
            "Iteration number");
    /* Close the element named xs:annotation, xs:element */
    writeXMLEndTag(writer, 2);
    writeXMLTagAndAttribute(writer,
            "xs:element", "ref", "xagent", "minOccurs", "0",
            "maxOccurs", "unbounded");
    /* Close the element named xs:element, xs:sequence,
     * xs:complexType, xs:element */
    writeXMLEndTag(writer, 4);
}

void IOXMLPop::createDataSchema(std::string const& file,
        flame::model::XModel * model) {
    std::vector<model::XMachine*>::iterator agent;
    std::vector<model::XVariable*>::iterator variable;
    /* The xml text writer */
    xmlTextWriterPtr writer;

#ifndef TESTBUILD
    printf("Writing file: '%s'\n", file.c_str());
#endif

    /* Open file to write to, with no compression */
    writer = xmlNewTextWriterFilename(file.c_str(), 0);
    if (writer == NULL)
        throw exc::flame_io_exception("Opening data schema file");
    /* Write tags on new lines */
    xmlTextWriterSetIndent(writer, 1);
    createDataSchemaHead(writer);
    createDataSchemaAgentNameType(writer, model);
    createDataSchemaAgentVarChoice(writer, model);
    createDataSchemaAgentVars(writer, model);
    createDataSchemaDefineAgents(writer);
    createDataSchemaDefineTags(writer);
    /* End xml file, automatically ends schema tag */
    endXMLDoc(writer);

    /* Free the xml writer */
    xmlFreeTextWriter(writer);
}

void IOXMLPop::openXMLDoc(xmlDocPtr * doc, std::string const& data_file) {
    *doc = xmlReadFile(data_file.c_str(), NULL, 0);
    /* Return error if the file was not successfully parsed */
    if (*doc == NULL)
        throw exc::flame_io_exception("Opening xml file");
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

void IOXMLPop::validateData(std::string const& data_file,
        std::string const& schema_file) {
    xmlDocPtr doc = NULL;
    xmlSchemaValidCtxtPtr valid_ctxt = NULL;
    xmlSchemaParserCtxtPtr parser_ctxt = NULL;
    xmlSchemaPtr schema = NULL;
    xmlDocPtr schema_doc = NULL;

    /* Try and open pop data xml */
    openXMLDoc(&doc, data_file);
    /* If successful try and open schema */
    openXMLSchema(&valid_ctxt, schema_file,
                &parser_ctxt, &schema, &schema_doc);

    /* 0 if valid, positive error code otherwise
     * -1 in case of internal or API error */
    xmlSchemaValidateDoc(valid_ctxt, doc);

    /* Free all pointers */
    xmlSchemaFreeValidCtxt(valid_ctxt);
    xmlSchemaFree(schema);
    xmlSchemaFreeParserCtxt(parser_ctxt);
    xmlFreeDoc(schema_doc);
    xmlFreeDoc(doc);
}

void IOXMLPop::processStartNode(std::vector<std::string> * tags,
        std::string name) {
    /* If correct tag at correct depth with
     * correct tag name */
    if ((tags->size() == 0 && name == "states") ||
        (tags->size() == 1 &&
        (name == "itno" || name == "environment" || name == "xagent")) ||
        tags->size() == 2) tags->push_back(name);
    else
    throw exc::unparseable_file(std::string("Unknown xml tag: ").append(name));
}

template <class T>
int IOXMLPop::processTextVariableCast(std::string value,
        std::vector<std::string> * tags, model::XMachine ** agent) {
    flame::mem::MemoryManager& memoryManager =
                    flame::mem::MemoryManager::GetInstance();
    T typeValue;
    // Try and cast string to type
    try {
        typeValue = boost::lexical_cast<T>(value);
    // Catch exception
    } catch(const boost::bad_lexical_cast&) {
        throw exc::invalid_pop_file(
            std::string("Variable could not be cast to correct type: ").
                append(value).append(" in ").append(tags->back()));
    }
    // Add value to memory manager
    std::vector<T>* vec =
        memoryManager.GetVector<T>(
            (*agent)->getName(), tags->back());
    vec->push_back(typeValue);

    return 0;
}

int IOXMLPop::processTextVariable(std::string value,
        std::vector<std::string> * tags, model::XMachine ** agent) {
    int rc;
    /* Get pointer to variable type */
    model::XVariable * var = (*agent)->getVariable(tags->back());
    /* Check if variable is part of the agent */
    if (var) {
        /* Check variable type for casting and
         * use appropriate casting function */
        if (var->getType() == "int") {
            rc = processTextVariableCast<int>(
                    value, tags, agent);
            if (rc != 0) return rc;
        } else if (var->getType() == "double") {
            rc = processTextVariableCast<double>(
                    value, tags, agent);
            if (rc != 0) return rc;
        }
    } else {
        throw exc::invalid_pop_file(
            std::string("Agent variable is not recognised: ").
                append(tags->back()));
    }

    return 0;
}

int IOXMLPop::processTextAgent(std::vector<std::string> * tags,
        xmlTextReaderPtr reader,
        model::XMachine ** agent, model::XModel * model) {
    int rc = 0;

    /* Read value */
    std::string value = reinterpret_cast<const char*>
            (xmlTextReaderConstValue(reader));
    /* If tag is the agent name */
    if (tags->back() == "name") {
        /* Check if agent is part of this model */
        (*agent) = model->getAgent(value);
        /* If agent name is unknown */
        if (!(*agent))
            throw exc::invalid_pop_file(
                std::string("Agent type is not recognised: ").append(value));
    } else { if (*agent) /* Check if agent exists */
        rc = processTextVariable(value, tags, agent);
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
            processStartNode(tags, name);
            break;
        case 3: /* Text */
            if (tags->size() == 3 && tags->at(1) == "xagent")
                rc = processTextAgent(
                    tags, reader, agent, model);
            break;
        case 15: /* End element */
            rc = processEndNode(tags, name, agent);
            break;
    }
    if (rc != 0) return rc;

    return 0;
}

void IOXMLPop::writeXMLEndTag(xmlTextWriterPtr writer) {
    int rc;
    rc = xmlTextWriterEndElement(writer);
    if (rc < 0)
        throw exc::flame_io_exception("Writing to xml file");
}

void IOXMLPop::writeXMLEndTag(xmlTextWriterPtr writer, int count) {
    int ii;
    for (ii = 0; ii < count; ++ii)
        writeXMLEndTag(writer);
}

void IOXMLPop::writeXMLTag(xmlTextWriterPtr writer, std::string name) {
    int rc;
    rc = xmlTextWriterStartElement(writer, BAD_CAST name.c_str());
    if (rc < 0)
        throw exc::flame_io_exception("Writing to xml file");
}

void IOXMLPop::writeXMLTag(xmlTextWriterPtr writer,
        std::string name, int value) {
    int rc;
    rc = xmlTextWriterWriteFormatElement(writer, BAD_CAST name.c_str(),
                                                 "%d", value);
    if (rc < 0)
        throw exc::flame_io_exception("Writing to xml file");
}

void IOXMLPop::writeXMLTag(xmlTextWriterPtr writer,
        std::string name, double value) {
    int rc;
    rc = xmlTextWriterWriteFormatElement(writer, BAD_CAST name.c_str(),
                                                 "%f", value);
    if (rc < 0)
        throw exc::flame_io_exception("Writing to xml file");
}

void IOXMLPop::writeXMLTag(xmlTextWriterPtr writer, std::string name,
        std::string value) {
    int rc;
    rc = xmlTextWriterWriteFormatElement(writer, BAD_CAST name.c_str(),
                                                 "%s", value.c_str());
    if (rc < 0)
        throw exc::flame_io_exception("Writing to xml file");
}

void IOXMLPop::writeXMLTagAttribute(xmlTextWriterPtr writer,
        std::string name, std::string value) {
    int rc;
    rc = xmlTextWriterWriteAttribute(writer,
            BAD_CAST name.c_str(), BAD_CAST value.c_str());
    if (rc < 0)
        throw exc::flame_io_exception("Writing to xml file");
}

void IOXMLPop::writeXMLTagAndAttribute(xmlTextWriterPtr writer, std::string tag,
        std::string name, std::string value) {
    // Write tag with 1 attribute
    writeXMLTag(writer, tag);
    writeXMLTagAttribute(writer, name, value);
}

void IOXMLPop::writeXMLTagAndAttribute(xmlTextWriterPtr writer, std::string tag,
        std::string name1, std::string value1,
        std::string name2, std::string value2) {
    // Write tag with 2 attributes
    writeXMLTag(writer, tag);
    writeXMLTagAttribute(writer, name1, value1);
    writeXMLTagAttribute(writer, name2, value2);
}

void IOXMLPop::writeXMLTagAndAttribute(xmlTextWriterPtr writer, std::string tag,
        std::string name1, std::string value1,
        std::string name2, std::string value2,
        std::string name3, std::string value3) {
    // Write tag with 3 attributes
    writeXMLTag(writer, tag);
    writeXMLTagAttribute(writer, name1, value1);
    writeXMLTagAttribute(writer, name2, value2);
    writeXMLTagAttribute(writer, name3, value3);
}

void IOXMLPop::endXMLDoc(xmlTextWriterPtr writer) {
    int rc = xmlTextWriterEndDocument(writer);
    if (rc < 0)
        throw exc::flame_io_exception("Closing xml file");
}

void IOXMLPop::setIteration(size_t i) {
    iteration_ = i;
}

}}}  // namespace flame::io::xml
