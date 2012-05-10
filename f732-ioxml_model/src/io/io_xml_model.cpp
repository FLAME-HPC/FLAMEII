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
#include <string>
#include <vector>
#include <cstdio>
#include "io_xml_model.hpp"

namespace model = flame::model;

namespace flame { namespace io { namespace xml {

static void
print_element_names(xmlNode * a_node)
{
    xmlNode *cur_node = NULL;

    for (cur_node = a_node; cur_node; cur_node = cur_node->next) {
        if (cur_node->type == XML_ELEMENT_NODE) {
            printf("node type: Element, name: %s\n", cur_node->name);
        }

        print_element_names(cur_node->children);
    }
}

std::string getElementName(xmlNode * node) {
    std::string s = reinterpret_cast<const char*>(node->name);
    return s;
}

void cleanUpXMLParser(xmlDoc *doc) {
    /* Free the document */
    xmlFreeDoc(doc);
    /* Free the global variables that may
     * have been allocated by the parser. */
    xmlCleanupParser();
}

int IOXMLModel::readXMLModel(std::string file_name, model::XModel * model) {
    /* Used for return codes */
    int rc;
    /* Directory of the model file */
    std::string directory;
    xmlDoc *doc = NULL;
    xmlNode *root_element = NULL;

    /* Find file directory to help open any submodels */
    boost::filesystem3::path filePath(file_name);
    directory = filePath.parent_path().string();
    directory.append("/");

    /* Print out diagnostics */
    fprintf(stdout, "Reading '%s'\n", file_name.c_str());

    /* Save absolute path to check the file is not read again */
    model->setPath(boost::filesystem3::absolute(filePath).string());

    /* Parse the file and get the DOM */
    doc = xmlReadFile(file_name.c_str(), NULL, 0);

    /* Check if file opened successfully */
    if (doc == NULL) {
        /* Return error if the file was not successfully parsed */
        std::fprintf(stderr,
                "Error: Model file cannot be opened/parsed: %s\n", file_name.c_str());
        return 1;
    }

    /*Get the root element node */
    root_element = xmlDocGetRootElement(doc);

    /* Catch error if no root called xmodel */
    if (getElementName(root_element) != "xmodel") {
        std::fprintf(stderr,
            "Error: Model file does not have root called 'xmodel': %s\n",
                file_name.c_str());
        cleanUpXMLParser(doc);
        return 3;
    }

    /* Catch error if version is not 2 */
    std::string version = reinterpret_cast<const char*>(
            xmlGetProp(root_element, (const xmlChar*)"version"));
    if (version != "2") {
        std::fprintf(stderr,
                "Error: Model file is not 'xmodel' version 2: %s\n",
                file_name.c_str());
        cleanUpXMLParser(doc);
        return 4;
    }

    //print_element_names(root_element);

    /* Loop through each child of xmodel */
    xmlNode *cur_node = NULL;
    for (cur_node = root_element->children; cur_node; cur_node = cur_node->next) {
        if (cur_node->type == XML_ELEMENT_NODE) {
            /* Handle each child */
            std::string name = getElementName(cur_node);
            if (name == "name") {
                model->setName(reinterpret_cast<const char*>(cur_node->children->content));
            } else if (name == "version") {
            } else if (name == "author") {
            } else if (name == "description") {
            } /*else if (v.first == "models") {
                rc = readIncludedModels(v, directory, model);
                if (rc != 0) return rc;
            } else if (v.first == "environment") {
                rc = readEnvironment(v, model);
                if (rc != 0) {
                    myfile.close();
                    return rc;
                }
            } else if (v.first == "agents") {
                rc = readAgents(v, model);
                if (rc != 0) {
                    myfile.close();
                    return rc;
                }
            } else if (v.first == "messages") {
                rc = readMessages(v, model);
                if (rc != 0) {
                    myfile.close();
                    return rc;
                }
            }*/ else {
                rc = readUnknownElement(cur_node);
                if (rc != 0) {
                    cleanUpXMLParser(doc);
                    return rc;
                }
            }
        }
    }

    /*
     *Free the global variables that may
     *have been allocated by the parser.
     */
     cleanUpXMLParser(doc);

    return 0;
}

int IOXMLModel::readUnknownElement(xmlNode * node) {
    std::fprintf(stderr,
            "Warning: Model file has unknown element '%s'\n", node->name);

    /* Return zero to carry on as normal */
    return 0;
}

}}}  // namespace flame::io::xml
