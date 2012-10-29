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
#include "./io_manager.hpp"

void printErr(std::string message);

namespace flame { namespace io {

namespace exc = flame::exceptions;

void IOManager::loadModel(std::string const& file,
        flame::model::XModel * model) {
    int rc;

    /* Read model */
    rc = ioxmlmodel.readXMLModel(file, model);
    if (rc != 0) throw exc::flame_io_exception("Model file cannot be loaded");
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

void IOManager::readPop(std::string file_name,
            model::XModel * model,
            FileType fileType) {
    int rc;
    std::string xmlpopxsd;

    if (fileType == xml) {
        /* Set path to xml pop location */
        ioxmlpop.setXmlPopPath(file_name);
        /* Validate xml first */
        xmlpopxsd = std::string(ioxmlpop.xmlPopPath()).append("xmlpop.xsd");
        /* Create data schema */
        ioxmlpop.createDataSchema(xmlpopxsd, model);
        /* Validate data using schema */
        ioxmlpop.validateData(file_name, xmlpopxsd);
        removeFile(xmlpopxsd);
        /* Read validated pop xml */
        rc = ioxmlpop.readPop(file_name, model);
        if (rc != 0)
            throw exc::flame_io_exception("Could not read pop file");
    } else {
        throw exc::flame_io_exception("unknown file type");
    }
}

void IOManager::writePop(std::string agent_name, std::string var_name) {
    ioxmlpop.writePop(agent_name, var_name);
}

void IOManager::initialiseData() {
    ioxmlpop.initialiseData();
}

void IOManager::finaliseData() {
    ioxmlpop.finaliseData();
}

void IOManager::setIteration(size_t i) {
    iteration_ = i;
    ioxmlpop.setIteration(i);
}

}}  // namespace flame::io