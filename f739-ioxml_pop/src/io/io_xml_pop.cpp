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

int writeXMLTag(xmlTextWriterPtr writer);
int writeXMLTag(xmlTextWriterPtr writer, std::string name);
int writeXMLTag(xmlTextWriterPtr writer, std::string name, int value);
int writeXMLTag(xmlTextWriterPtr writer, std::string name, double value);
int writeXMLTag(xmlTextWriterPtr writer, std::string name, std::string value);
int writeXMLTagAttribute(xmlTextWriterPtr writer,
        std::string name, std::string value);
int endXMLDoc(xmlTextWriterPtr writer);

typedef std::vector<int>* intVecPtr;
typedef std::vector<double>* doubleVecPtr;

IOXMLPop::IOXMLPop() {
    xml_pop_path_is_set = false;
}

int IOXMLPop::writeXMLPop(std::string file_name,
        int iterationNo,
        model::XModel * model,
        flame::mem::MemoryManager * memoryManager) {
    /* Return code */
    int rc;
    /* The xml text writer */
    xmlTextWriterPtr writer;
    /* Loop variables */
    size_t ii, jj, kk;
    /* The number of agents per agent type */
    size_t noAgents;
    /* List of memory vector readers populated for each agent */
    std::vector< boost::variant<intVecPtr, doubleVecPtr> > varVectors;

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
        /* Assign to local agent variable */
        model::XMachine * agent = model->getAgents()->at(ii);
        /* For each memory variable */
        for (jj = 0;
            jj < agent->getVariables()->size(); jj++) {
            /* Assign to local xvariable variable */
            model::XVariable * var = agent->getVariables()->at(jj);

            /* Set up vector readers for each memory variable
             * dependent on variable data type and add to a
             * list ready to be used to write each agents
             * memory out in one go.
             */
            if (var->getType() == "int") {
                /* Create vector reader.. */
                intVecPtr roi =
                        memoryManager->GetVector<int>(
                                agent->getName(), var->getName());
                /* ..and add to list of vectors. */
                varVectors.push_back(roi);
                /* Check array length */
                if (jj == 0) noAgents = roi->size();
                else if (roi->size() != noAgents) {
    fprintf(stderr, "Error: Memory vector size does not correspond: '%s'\n",
                        var->getName().c_str());
                    return 3;
                }
            }
            if (var->getType() == "double") {
                /* Create vector reader.. */
                doubleVecPtr rod =
                        memoryManager->GetVector<double>(
                                agent->getName(), var->getName());
                /* ..and add to list of vectors. */
                varVectors.push_back(rod);
                /* Check array length */
                if (jj == 0) noAgents = rod->size();
                else if (rod->size() != noAgents) {
    fprintf(stderr, "Error: Memory vector size does not correspond: '%s'\n",
                        var->getName().c_str());
                    return 3;
                }
            }
        }

        /* For each agent in the simulation */
        for (kk = 0; kk < noAgents; kk++) {
            /* Open root tag */
            rc = writeXMLTag(writer, "xagent");
            if (rc != 0) return rc;

            /* Write agent name */
            rc = writeXMLTag(writer, "name", agent->getName());
            if (rc != 0) return rc;

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
                            varVectors.at(jj))->begin()+kk));
                    if (rc != 0) return rc;
                }
                if (var->getType() == "double") {
                    rc = writeXMLTag(writer, var->getName(),
                        *(boost::get<doubleVecPtr>(
                        varVectors.at(jj))->begin()+kk) );
                    if (rc != 0) return rc;
                }
            }

            /* Close the element named xagent. */
            writeXMLTag(writer);
        }
        /* Clear the memory vector reader list for the next agent type */
        varVectors.clear();
    }

    /* End xml file, automatically ends states tag */
    rc = endXMLDoc(writer);
    if (rc != 0) return rc;

    /* Free the xml writer */
    xmlFreeTextWriter(writer);

    return 0;
}

int IOXMLPop::readXMLPop(std::string file_name, model::XModel * model,
        flame::mem::MemoryManager * memoryManager) {
    xmlTextReaderPtr reader;
    int ret, rc;
    /* Using vector instead of stack as need to access earlier tags */
    std::vector<std::string> tags;
    /* Pointer to current agent type, 0 if invalid */
    model::XMachine * agent = 0;

    /* Open file to read */
    reader = xmlReaderForFile(file_name.c_str(), NULL, 0);
    /* Check if file opened successfully */
    if (reader != NULL) {
        /* Read the first node */
        ret = xmlTextReaderRead(reader);
        /* Continue reading nodes until end */
        while (ret == 1) {
            /* Process node */
            rc = processNode(reader, model, memoryManager,
                    &tags, &agent);
            /* If error clean up and return */
            if (rc != 0) {
                xmlFreeTextReader(reader);
                return rc;
            }
            /* Read next node */
            ret = xmlTextReaderRead(reader);
        }
        /* Clean up */
        xmlFreeTextReader(reader);
        /* If error reading node return */
        if (ret != 0) {
            fprintf(stderr, "Error: Failed to parse: '%s'\n",
                file_name.c_str());
            return 2;
        }
    } else {
        /* If file not opened successfully */
        fprintf(stderr, "Error: Unable to open: '%s'\n", file_name.c_str());
        return 1;
    }

    /* Set the xml pop path to the directory of the opened file.
     * This path is then used as the root directory to write xml pop to. */
    boost::filesystem::path p(file_name);
    boost::filesystem::path dir = p.parent_path();
    xml_pop_path = dir.string();
    xml_pop_path.append("/");
    xml_pop_path_is_set = true;
    printf("%s %s\n", file_name.c_str(), xml_pop_path.c_str());

    /* Return successfully */
    return 0;
}

bool IOXMLPop::xmlPopPathIsSet() {
    return xml_pop_path_is_set;
}

int IOXMLPop::createDataSchema(std::string const& file,
        flame::model::XModel * model) {
    /* Return code */
    int rc;
    std::vector<model::XMachine*>::iterator agent;
    std::vector<model::XVariable*>::iterator variable;
    /* The xml text writer */
    xmlTextWriterPtr writer;

    /* Open file to write to, with no compression */
    writer = xmlNewTextWriterFilename(file.c_str(), 0);
    if (writer == NULL) {
        fprintf(stderr, "Error: Opening data schema file to write to\n");
        return 1;
    }
    /* Write tags on new lines */
    xmlTextWriterSetIndent(writer, 1);

    /* Open root tag */
    rc = writeXMLTag(writer, "xs:schema");
    if (rc != 0) return rc;

    rc = writeXMLTagAttribute(writer, "xmlns:xs",
            "http://www.w3.org/2001/XMLSchema");
    if (rc != 0) return rc;

    rc = writeXMLTagAttribute(writer, "elementFormDefault", "qualified");
    if (rc != 0) return rc;

    rc = writeXMLTag(writer, "xs:annotation");
    if (rc != 0) return rc;

    rc = writeXMLTag(writer, "xs:documentation", "Auto-generated data schema");
    if (rc != 0) return rc;

    /* Close the element named xs:annotation. */
    writeXMLTag(writer);

    /* Define agent name type */

    rc = writeXMLTag(writer, "xs:simpleType");
    if (rc != 0) return rc;

    rc = writeXMLTagAttribute(writer, "name", "agentType");
    if (rc != 0) return rc;

    rc = writeXMLTag(writer, "xs:restriction");
    if (rc != 0) return rc;

    rc = writeXMLTagAttribute(writer, "base", "xs:string");
    if (rc != 0) return rc;

    for (agent = model->getAgents()->begin();
            agent != model->getAgents()->end();
            agent++) {
        rc = writeXMLTag(writer, "xs:enumeration");
        if (rc != 0) return rc;

        rc = writeXMLTagAttribute(writer, "value", (*agent)->getName());
        if (rc != 0) return rc;

        /* Close the element named xs:enumeration. */
        writeXMLTag(writer);
    }

    /* Close the element named xs:restriction. */
    writeXMLTag(writer);

    /* Close the element named xs:simpleType. */
    writeXMLTag(writer);

    /* Define agent variables */
    rc = writeXMLTag(writer, "xs:group");
    if (rc != 0) return rc;
    rc = writeXMLTagAttribute(writer, "name", "agent_vars");
    if (rc != 0) return rc;
    rc = writeXMLTag(writer, "xs:choice");
    if (rc != 0) return rc;

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
        writeXMLTag(writer);
    }

    /* Close the element named xs:choice. */
    writeXMLTag(writer);
    /* Close the element named xs:group. */
    writeXMLTag(writer);

    for (agent = model->getAgents()->begin();
                agent != model->getAgents()->end();
                agent++) {
        rc = writeXMLTag(writer, "xs:group");
        if (rc != 0) return rc;
        std::string name = "agent_";
        name.append((*agent)->getName());
        name.append("_vars");
        rc = writeXMLTagAttribute(writer, "name", name);
        if (rc != 0) return rc;
        rc = writeXMLTag(writer, "xs:sequence");
        if (rc != 0) return rc;
        for (variable = (*agent)->getVariables()->begin();
                variable != (*agent)->getVariables()->end();
                variable++) {
            rc = writeXMLTag(writer, "xs:element");
            if (rc != 0) return rc;
            rc = writeXMLTagAttribute(writer, "name", (*variable)->getName());
            if (rc != 0) return rc;
            std::string type;
            if ((*variable)->getType() == "int") type = "xs:integer";
            else if ((*variable)->getType() == "double") type = "xs:double";
            else
                type = "xs:string";
            rc = writeXMLTagAttribute(writer, "type", type);
            if (rc != 0) return rc;
            /* Close the element named xs:element. */
            writeXMLTag(writer);
        }
        /* Close the element named xs:sequence. */
        writeXMLTag(writer);
        /* Close the element named xs:group. */
        writeXMLTag(writer);
    }

    /* Define agents */

    rc = writeXMLTag(writer, "xs:element");
    if (rc != 0) return rc;

    rc = writeXMLTagAttribute(writer, "name", "xagent");
    if (rc != 0) return rc;

    rc = writeXMLTag(writer, "xs:annotation");
    if (rc != 0) return rc;

    rc = writeXMLTag(writer, "xs:documentation", "Agent data");
    if (rc != 0) return rc;

    /* Close the element named xs:annotation. */
    writeXMLTag(writer);

    rc = writeXMLTag(writer, "xs:complexType");
        if (rc != 0) return rc;

    rc = writeXMLTag(writer, "xs:sequence");
    if (rc != 0) return rc;

    rc = writeXMLTag(writer, "xs:element");
    if (rc != 0) return rc;

    rc = writeXMLTagAttribute(writer, "name", "name");
    if (rc != 0) return rc;

    rc = writeXMLTagAttribute(writer, "type", "agentType");
    if (rc != 0) return rc;

    /* Close the element named xs:element. */
    writeXMLTag(writer);

    rc = writeXMLTag(writer, "xs:group");
    if (rc != 0) return rc;

    rc = writeXMLTagAttribute(writer, "ref", "agent_vars");
    if (rc != 0) return rc;

    /* Close the element named xs:group. */
    writeXMLTag(writer);

    /* Close the element named xs:sequence. */
    writeXMLTag(writer);

    /* Close the element named xs:complexType. */
    writeXMLTag(writer);

    /* Close the element named xs:element. */
    writeXMLTag(writer);

    /* XML Tag structure */

    rc = writeXMLTag(writer, "xs:element");
    if (rc != 0) return rc;

    rc = writeXMLTagAttribute(writer, "name", "states");
    if (rc != 0) return rc;

    rc = writeXMLTag(writer, "xs:complexType");
    if (rc != 0) return rc;

    rc = writeXMLTag(writer, "xs:sequence");
    if (rc != 0) return rc;

    rc = writeXMLTag(writer, "xs:element");
    if (rc != 0) return rc;

    rc = writeXMLTagAttribute(writer, "name", "itno");
    if (rc != 0) return rc;

    rc = writeXMLTagAttribute(writer, "type", "xs:nonNegativeInteger");
    if (rc != 0) return rc;

    rc = writeXMLTag(writer, "xs:annotation");
    if (rc != 0) return rc;

    rc = writeXMLTag(writer, "xs:documentation", "Iteration number");
    if (rc != 0) return rc;

    /* Close the element named xs:annotation. */
    writeXMLTag(writer);

    /* Close the element named xs:element. */
    writeXMLTag(writer);

    rc = writeXMLTag(writer, "xs:element");
    if (rc != 0) return rc;

    rc = writeXMLTagAttribute(writer, "ref", "xagent");
    if (rc != 0) return rc;

    rc = writeXMLTagAttribute(writer, "minOccurs", "0");
    if (rc != 0) return rc;

    rc = writeXMLTagAttribute(writer, "maxOccurs", "unbounded");
    if (rc != 0) return rc;

    /* Close the element named xs:element. */
    writeXMLTag(writer);

    /* Close the element named xs:sequence. */
    writeXMLTag(writer);

    /* Close the element named xs:complexType. */
    writeXMLTag(writer);

    /* Close the element named xs:element. */
    writeXMLTag(writer);

    /* End xml file, automatically ends schema tag */
    rc = endXMLDoc(writer);
    if (rc != 0) return rc;

    /* Free the xml writer */
    xmlFreeTextWriter(writer);

    return 0;
}

int IOXMLPop::validateData(std::string const& data_file,
        std::string const& schema_file) {
    xmlDocPtr schema_doc = xmlReadFile(schema_file.c_str(), NULL,
            XML_PARSE_NONET);
    if (schema_doc == NULL) {
        /* the schema cannot be loaded or is not well-formed */
        return -1;
    }
    xmlSchemaParserCtxtPtr parser_ctxt = xmlSchemaNewDocParserCtxt(schema_doc);
    if (parser_ctxt == NULL) {
        /* unable to create a parser context for the schema */
        xmlFreeDoc(schema_doc);
        return -2;
    }
    xmlSchemaPtr schema = xmlSchemaParse(parser_ctxt);
    if (schema == NULL) {
        /* the schema itself is not valid */
        xmlSchemaFreeParserCtxt(parser_ctxt);
        xmlFreeDoc(schema_doc);
        return -3;
    }
    xmlSchemaValidCtxtPtr valid_ctxt = xmlSchemaNewValidCtxt(schema);
    if (valid_ctxt == NULL) {
        /* unable to create a validation context for the schema */
        xmlSchemaFree(schema);
        xmlSchemaFreeParserCtxt(parser_ctxt);
        xmlFreeDoc(schema_doc);
        return -4;
    }

    xmlDocPtr doc = xmlReadFile(data_file.c_str(), NULL, 0);
    if (doc == NULL) {
        /* Return error if the file was not successfully parsed */
        std::fprintf(stderr,
                "Error: Data file cannot be opened/parsed: %s\n",
                data_file.c_str());
        return -5;
    }

    /* 0 if valid, positive error code otherwise
     * -1 in case of internal or API error */
    int rc = xmlSchemaValidateDoc(valid_ctxt, doc);
    xmlSchemaFreeValidCtxt(valid_ctxt);
    xmlSchemaFree(schema);
    xmlSchemaFreeParserCtxt(parser_ctxt);
    xmlFreeDoc(schema_doc);
    xmlFreeDoc(doc);

    return rc;
}

int IOXMLPop::processNode(xmlTextReaderPtr reader,
        model::XModel * model,
        flame::mem::MemoryManager * memoryManager,
        std::vector<std::string> * tags,
        model::XMachine ** agent) {
    /* Node name */
    const xmlChar *xmlname;
    /* Node name as strings */
    std::string name;

    /* Read node name */
    xmlname = xmlTextReaderConstName(reader);
    if (xmlname == NULL)
    xmlname = BAD_CAST "--";

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
            /* If correct tag at correct depth */
            if ((tags->size() == 0 && name == "states") ||
                (tags->size() == 1 && name == "itno") ||
                (tags->size() == 1 && name == "environment") ||
                (tags->size() == 1 && name == "xagent") ||
                tags->size() == 2) {
                tags->push_back(name);
            } else {
                fprintf(stderr, "Error: Unknown tag: '%s'\n",
                        name.c_str());
                return 3;
            }
            break;
        case 3: /* Text */
            if (tags->size() == 3 && tags->at(1) == "xagent") {
                /* Read value */
                std::string value =
                    reinterpret_cast<const char*>
                        (xmlTextReaderConstValue(reader));
                /* If tag is the agent name */
                if (tags->back() == "name") {
                    /* Check if agent is part of this model */
                    (*agent) = model->getAgent(value);
                    /* If agent name is unknown */
                    if (!(*agent)) {
                fprintf(stderr, "Error: Agent type is not recognised: '%s'\n",
                            value.c_str());
                        return 4;
                    }
                } else {
                    /* Check if agent exists */
                    if ((*agent)) {
                        model::XVariable * var =
                            (*agent)->getVariable(tags->back());
                        /* Check if variable is part of the agent */
                        if (var) {
                            /* Check variable type for casting */
                            if (var->getType() == "int") {
                                int intValue;
                                try {
                                    intValue = boost::lexical_cast<int>(value);
                                } catch(const boost::bad_lexical_cast&) {
                                    std::fprintf(stderr,
                    "Error: int variable is not an integer: '%s' in '%s'\n",
                                        value.c_str(),
                                        tags->back().c_str());
                                    return 6;
                                }
                                /* Add value to memory manager */
                                std::vector<int>* vec =
                                    memoryManager->GetVector<int>(
                                        (*agent)->getName(), tags->back());
                                vec->push_back(intValue);
                            } else if (var->getType() == "double") {
                                double doubleValue;
                                try {
                                    doubleValue =
                                            boost::lexical_cast<double>(value);
                                } catch(const boost::bad_lexical_cast&) {
                                    std::fprintf(stderr,
                    "Error: double variable is not a double: '%s' in '%s'\n",
                                        value.c_str(),
                                        tags->back().c_str());
                                    return 6;
                                }
                                /* Add value to memory manager */
                                std::vector<double>* vec =
                                    memoryManager->
                                        GetVector<double>(
                                            (*agent)->getName(), tags->back());
                                vec->push_back(doubleValue);
                            }
                        } else {
            fprintf(stderr, "Error: Agent variable is not recognised: '%s'\n",
                            tags->back().c_str());
                            return 5;
                        }
                    }
                }
            }
            break;
        case 15: /* End element */
            /* Check end tag closes opened tag */
            if (tags->back() == name) {
                /* If end of agent then reset agent pointer */
                if (name == "xagent") *agent = 0;
                tags->pop_back();
            } else {
                /* This error is handled by the xml library and
                 * should never be reached */
        fprintf(stderr, "Error: Tag is not closed properly: '%s' with '%s'\n",
                        name.c_str(),
                        tags->back().c_str());
                return 9;
            }
            break;
    }

    return 0;
}

int writeXMLTag(xmlTextWriterPtr writer) {
    int rc;
    rc = xmlTextWriterEndElement(writer);
    if (rc < 0) {
        fprintf(stderr, "Error: Writing to xml file\n");
        return 2;
    }
    return 0;
}

int writeXMLTag(xmlTextWriterPtr writer, std::string name) {
    int rc;
    rc = xmlTextWriterStartElement(writer, BAD_CAST name.c_str());
    if (rc < 0) {
        fprintf(stderr, "Error: Writing to xml file\n");
        return 2;
    }
    return 0;
}

int writeXMLTag(xmlTextWriterPtr writer, std::string name, int value) {
    int rc;
    rc = xmlTextWriterWriteFormatElement(writer, BAD_CAST name.c_str(),
                                                 "%d", value);
    if (rc < 0) {
        fprintf(stderr, "Error: Writing to xml file\n");
        return 2;
    }
    return 0;
}

int writeXMLTag(xmlTextWriterPtr writer, std::string name, double value) {
    int rc;
    rc = xmlTextWriterWriteFormatElement(writer, BAD_CAST name.c_str(),
                                                 "%f", value);
    if (rc < 0) {
        fprintf(stderr, "Error: Writing to xml file\n");
        return 2;
    }
    return 0;
}

int writeXMLTag(xmlTextWriterPtr writer, std::string name, std::string value) {
    int rc;
    rc = xmlTextWriterWriteFormatElement(writer, BAD_CAST name.c_str(),
                                                 "%s", value.c_str());
    if (rc < 0) {
        fprintf(stderr, "Error: Writing to xml file\n");
        return 2;
    }
    return 0;
}

int writeXMLTagAttribute(xmlTextWriterPtr writer,
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

int endXMLDoc(xmlTextWriterPtr writer) {
    int rc;
    rc = xmlTextWriterEndDocument(writer);
    if (rc < 0) {
        fprintf(stderr, "Error: Writing to xml file\n");
        return 2;
    }
    return 0;
}

}}}  // namespace flame::io::xml
