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
#include <libxml/xmlreader.h>
#include <string>
#include <vector>
#include <cstdio>
#include "./io_xml_pop.hpp"

namespace flame { namespace io {

int IOXMLPop::readXMLPop(std::string file_name, XModel * model,
        flame::mem::MemoryManager * memoryManager) {
    xmlTextReaderPtr reader;
    int ret, rc;
    /* Using vector instead of stack as need to access earlier tags */
    std::vector<std::string> tags;
    /* Pointer to current agent type, 0 if invalid */
    XMachine * agent = 0;

    /* Open file to read */
    reader = xmlReaderForFile(file_name.c_str(), NULL, 0);
    /* Check if file opened successfully */
    if (reader != NULL) {
        /* Read the first node */
        ret = xmlTextReaderRead(reader);
        /* Continue reading nodes until end */
        while (ret == 1) {
            /* Process node */
            rc = processNode(reader, model, memoryManager, &tags, &agent);
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

    /* Return successfully */
    return 0;
}

int IOXMLPop::processNode(xmlTextReaderPtr reader,
        XModel * model,
        flame::mem::MemoryManager * memoryManager,
        std::vector<std::string> * tags,
        XMachine ** agent) {
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
                        XVariable * var = (*agent)->getVariable(tags->back());
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
                                std::vector<int> &vec =
                                    memoryManager->GetMemoryVector_test<int>(
                                        (*agent)->getName(), tags->back());
                                vec.push_back(intValue);
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
                                std::vector<double> &vec =
                                    memoryManager->
                                        GetMemoryVector_test<double>(
                                            (*agent)->getName(), tags->back());
                                vec.push_back(doubleValue);
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

}}  // namespace flame::io
