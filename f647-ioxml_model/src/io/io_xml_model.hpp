/*!
 * \file src/io/io_xml_model.hpp
 * \author Simon Coakley
 * \date 2012
 * \copyright Copyright (c) 2012 STFC Rutherford Appleton Laboratory
 * \copyright Copyright (c) 2012 University of Sheffield
 * \copyright GNU Lesser General Public License
 * \brief IOXMLModel: reading of model XML file
 */
#ifndef IO__XML_MODEL_HPP_
#define IO__XML_MODEL_HPP_
#include <string>

namespace flame { namespace io {

class IOXMLModel {
  public:
    IOXMLModel() {}

    int readFile(std::string file_name);

  private:

};
}}  // namespace flame::io
#endif  // IO__XML_MODEL_HPP_
