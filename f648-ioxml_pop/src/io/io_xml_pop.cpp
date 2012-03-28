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
#include <libxml/xmlreader.h>
#include <fstream>
#include <string>
#include <vector>
#include <cstdio>
#include "./io_xml_pop.hpp"

namespace flame { namespace io {

int IOXMLPop::readXMLPop(std::string file_name, XModel * model) {
    xmlTextReaderPtr reader;
        int ret;

        reader = xmlReaderForFile(file_name.c_str(), NULL, 0);
        if (reader != NULL) {
            ret = xmlTextReaderRead(reader);
            while (ret == 1) {
                processNode(reader);
                ret = xmlTextReaderRead(reader);
            }
            xmlFreeTextReader(reader);
            if (ret != 0) {
                fprintf(stderr, "%s : failed to parse\n", file_name.c_str());
            }
        } else {
            fprintf(stderr, "Unable to open %s\n", file_name.c_str());
        }


    return 0;
}

void IOXMLPop::processNode(xmlTextReaderPtr reader) {
    const xmlChar *name, *value;

    name = xmlTextReaderConstName(reader);
    if (name == NULL)
    name = BAD_CAST "--";

    value = xmlTextReaderConstValue(reader);

    printf("%d %d %s %d %d",
        xmlTextReaderDepth(reader),
        xmlTextReaderNodeType(reader),
        name,
        xmlTextReaderIsEmptyElement(reader),
        xmlTextReaderHasValue(reader));
    if (value == NULL)
    printf("\n");
    else {
        if (xmlStrlen(value) > 40)
            printf(" %.40s...\n", value);
        else
        printf(" %s\n", value);
    }
}

}}  // namespace flame::io
