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
#include <libxml/xmlreader.h>
#include <string>
#include <vector>
#include <cstdio>
#include "io_xml_model.hpp"

namespace model = flame::model;

namespace flame { namespace io { namespace xml {

int processNode(xmlTextReaderPtr reader) {
    /* Node name */
    const xmlChar *xmlname;
    /* Node name as strings */
    std::string name;
    std::string value;
    int nodeType = xmlTextReaderNodeType(reader);

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
    if (nodeType == 1) { /* Start element */
            printf("START %s\n", name.c_str());
    } else if (nodeType == 3) {  /* Text */
            /* Read value */
            value =
                    reinterpret_cast<const char*>
                        (xmlTextReaderConstValue(reader));
            printf("      %s\n", value.c_str());
    } else if (nodeType == 15) {  /* End element */
            printf("FINISH %s\n", name.c_str());
    }

    return 0;
}

void cleanUpXMLParser(xmlDoc *doc) {
    /* Free the document */
    xmlFreeDoc(doc);
    /* Free the global variables that may
     * have been allocated by the parser. */
    xmlCleanupParser();
}

std::string getElementName(xmlTextReaderPtr reader) {
    std::string s = reinterpret_cast<const char*>(xmlTextReaderConstName(reader));
    return s;
}

std::string getElementValue(xmlTextReaderPtr reader) {
    std::string s = reinterpret_cast<const char*>(xmlTextReaderConstValue(reader));
    return s;
}

std::string getValuefromNameElement(xmlTextReaderPtr reader) {
    /* Read next element */
    if(xmlTextReaderRead(reader) == 1) {
        /* If value element */
        if(xmlTextReaderNodeType(reader) == 3) {
            return getElementValue(reader);
        } else { return 0; }
    } else { return 0; }
}

int IOXMLModel::readXMLModel(std::string file_name, model::XModel * model) {
    int ret, rc;
    /* Directory of the model file */
    std::string directory;
    xmlTextReaderPtr reader;

    /* Find file directory to help open any submodels */
    boost::filesystem3::path filePath(file_name);
    directory = filePath.parent_path().string();
    directory.append("/");

    /* Print out diagnostics */
    fprintf(stdout, "Reading '%s'\n", file_name.c_str());

    /* Save absolute path to check the file is not read again */
    model->setPath(boost::filesystem3::absolute(filePath).string());

    /* Open file to read */
    reader = xmlReaderForFile(file_name.c_str(), NULL, 0);

    /* Check if file opened successfully */
    if (reader != NULL) {
        /* Read the root node */
        ret = xmlTextReaderRead(reader);

        /* If root node exists */
        if (ret == 1) {
            /* Catch error if no root called xmodel */
            if (strcmp(reinterpret_cast<const char*>
                    (xmlTextReaderConstName(reader)),
                    "xmodel") != 0) {
                std::fprintf(stderr,
                    "Error: Model file does not have root called 'xmodel': %s\n",
                        file_name.c_str());
                xmlFreeTextReader(reader);
                return 3;
            }

            /* Catch error if version is not 2 */
            xmlChar * version = xmlTextReaderGetAttribute(reader,
                                (const xmlChar *)"version");
            if (version == NULL) {
                std::fprintf(stderr,
                    "Error: Model file does not have an 'xmodel' version to check: %s\n",
                    file_name.c_str());
                free(version);
                xmlFreeTextReader(reader);
                return 4;
            } else {
                if (strcmp(reinterpret_cast<const char*>(version), "2") != 0) {
                    std::fprintf(stderr,
                        "Error: Model file is not 'xmodel' version 2: %s\n",
                        file_name.c_str());
                    free(version);
                    xmlFreeTextReader(reader);
                    return 4;
                }
            }
            free(version);
        }

        /* Read the first root node child */
        ret = xmlTextReaderRead(reader);

        /* Continue reading nodes until end */
        while (ret == 1) {
            /* Set return code to be successful as
             * unsuccessful codes are handled at the end. */
            rc = 0;
            /* If start element */
            if(xmlTextReaderNodeType(reader) == 1) {
                std::string name = getElementName(reader);
                if (name == "name") {
                    model->setName(getValuefromNameElement(reader));
                } else if (name == "version") {
                } else if (name == "author") {
                } else if (name == "description") {
                } else if (name == "models") {
                    //rc = readIncludedModels(reader, directory, model);
                } else if (name == "environment") {
                    //rc = readEnvironment(reader, model);
                } else if (name == "agents") {
                    //rc = readAgents(reader, model);
                } else if (name == "messages") {
                    //rc = readMessages(reader, model);
                } else {
                    rc = readUnknownElement(reader);
                }
            }

            /* Handle return code errors */
            if(rc != 0) {
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
        return 0;
    } else {
        /* Return error if the file was not successfully parsed */
        std::fprintf(stderr,
                "Error: Model file cannot be opened: %s\n", file_name.c_str());
        return 1;
    }
}

int IOXMLModel::readUnknownElement(xmlTextReaderPtr reader) {
    std::fprintf(stderr,
            "Warning: Model file has unknown child '%s'\n", xmlTextReaderConstName(reader));

    /* Ignore all child elements */
    std::string name = getElementName(reader);
    int ret;
    /* Read the first node */
    ret = xmlTextReaderRead(reader);
    /* Find closing element */
    while (ret == 1 &&
            !(xmlTextReaderNodeType(reader) == 15 &&
            getElementName(reader) == name)) {
        /* Read next node */
        ret = xmlTextReaderRead(reader);
    }
    if (ret != 0 && ret != 1) {
        return 2;
    }

    /* Return zero to carry on as normal */
    return 0;
}

}}}  // namespace flame::io::xml
