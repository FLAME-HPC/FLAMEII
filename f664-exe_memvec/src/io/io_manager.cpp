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
#include "./io_xml_model.hpp"
#include "./io_xml_pop.hpp"

namespace flame { namespace io {

int IOManager::loadModel(std::string const& file,
        flame::model::XModel * model) {
    int rc;
    xml::IOXMLModel ioxmlmodel;

    rc = ioxmlmodel.readXMLModel(file, model);

    return rc;
}

}}  // namespace flame::io
