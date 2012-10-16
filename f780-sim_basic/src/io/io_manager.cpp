/*!
 * \file src/io/io_manager.cpp
 * \author Simon Coakley
 * \date 2012
 * \copyright Copyright (c) 2012 STFC Rutherford Appleton Laboratory
 * \copyright Copyright (c) 2012 University of Sheffield
 * \copyright GNU Lesser General Public License
 * \brief IOManager: management for I/O Backend
 */
#include <string>
#include "exceptions/io.hpp"
#include "./io_manager.hpp"

void printErr(std::string message);

namespace flame { namespace io {

int IOManager::loadModel(std::string const& file,
        flame::model::XModel * model) {
    int rc;

    /* Read model */
    rc = ioxmlmodel.readXMLModel(file, model);
    if (rc != 0) return rc;

    return rc;
}

int removeFile(std::string file) {
    if (remove(file.c_str()) != 0) {
        printErr(std::string("Warning: Could not delete the file: ") +
                file);
        return 1;
    }
#ifndef TESTBUILD
    printf("Removing file: %s\n", file.c_str());
#endif

    return 0;
}

int IOManager::readPop(std::string file_name,
            model::XModel * model,
            FileType fileType) {
    int rc;
    std::string xmlpopxsd;
    flame::mem::MemoryManager& memoryManager =
                flame::mem::MemoryManager::GetInstance();

    if (fileType == xml) {
        /* Set path to xml pop location */
        ioxmlpop.setXmlPopPath(file_name);
        /* Validate xml first */
        xmlpopxsd = std::string(ioxmlpop.xmlPopPath()).append("xmlpop.xsd");
        /* Create data schema */
        rc = ioxmlpop.createDataSchema(xmlpopxsd, model);
        if (rc != 0) {
            fprintf(stderr, "Error: Could not create data schema\n");
            return rc;
        }
        /* Validate data using schema */
        rc = ioxmlpop.validateData(file_name, xmlpopxsd);
        if (rc != 0) {
            printErr("Error: Could not validate data with schema\n");
            removeFile(xmlpopxsd);
            return rc;
        }
        removeFile(xmlpopxsd);
        /* Read validated pop xml */
        return ioxmlpop.readXMLPop(file_name, model, &memoryManager);
    } else {
        throw flame::exceptions::flame_io_exception("unknown file type");
    }
}

int IOManager::writePop(std::string file_name,
            int iterationNo,
            model::XModel * model,
            FileType fileType) {
    int rc;
    flame::mem::MemoryManager& memoryManager =
                flame::mem::MemoryManager::GetInstance();

    /* If file type selected is xml */
    if (fileType == xml) {
        /* Check a path has been set */
        if (!ioxmlpop.xmlPopPathIsSet()) {
            throw flame::exceptions::flame_io_exception("path not set");
        }
        /* Write out pop to xml */
        rc = ioxmlpop.writeXMLPop(file_name,
                iterationNo, model, &memoryManager);
        return rc;
    /* Through exception for unknown file types */
    } else {
        throw flame::exceptions::flame_io_exception("unknown file type");
    }
}

}}  // namespace flame::io
