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
#include "../mem/memory_manager.hpp"

namespace flame { namespace io {

int IOManager::loadModel(std::string const& file,
        flame::model::XModel * model) {
    int rc;
    size_t pop_size_hint = 100;
    flame::mem::MemoryManager& memoryManager =
                flame::mem::MemoryManager::GetInstance();

    /* Read model */
    rc = ioxmlmodel.readXMLModel(file, model);
    if (rc != 0) return rc;

    /* Register agents with memory manager */
    unsigned int ii, jj;
    for (ii = 0; ii < model->getAgents()->size(); ii++) {
        flame::model::XMachine * agent = model->getAgents()->at(ii);
        /* Register agent */
        memoryManager.RegisterAgent(agent->getName());
        /* Register agent memory variables */
        for (jj = 0; jj < agent->getVariables()->size(); jj++) {
            flame::model::XVariable * var =
                    agent->getVariables()->at(jj);
            if (var->getType() == "int") {
                /* Register int variable */
                memoryManager.RegisterAgentVar<int>
                    (agent->getName(), var->getName());
            } else if (var->getType() == "double") {
                /* Register double variable */
                memoryManager.RegisterAgentVar<double>
                    (agent->getName(), var->getName());
            }
        }
        /* Population Size hint */
        memoryManager.HintPopulationSize(agent->getName(), pop_size_hint);
    }

    return rc;
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
        xmlpopxsd = ioxmlpop.xmlPopPath();
        xmlpopxsd.append("xmlpop.xsd");
        /* Create data schema */
        rc = ioxmlpop.createDataSchema(xmlpopxsd, model);
        if (rc != 0) {
            fprintf(stderr, "Error: Could not create data schema\n");
            return rc;
        }
        /* Validate data using schema */
        rc = ioxmlpop.validateData(file_name, xmlpopxsd);
        if (rc != 0) {
            fprintf(stderr, "Error: Could not validate data with schema\n");
            return rc;
        }
        /* Read validated pop xml */
        rc = ioxmlpop.readXMLPop(file_name, model, &memoryManager);
        return rc;
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
            fprintf(stderr, "Error: Path not set by reading pop data\n");
            return -1;
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
