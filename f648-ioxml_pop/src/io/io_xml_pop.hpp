/*!
 * \file src/io/io_xml_pop.hpp
 * \author Simon Coakley
 * \date 2012
 * \copyright Copyright (c) 2012 STFC Rutherford Appleton Laboratory
 * \copyright Copyright (c) 2012 University of Sheffield
 * \copyright GNU Lesser General Public License
 * \brief IOXMLPop: reading of population XML file
 */
#ifndef IO__XML_POP_HPP_
#define IO__XML_POP_HPP_
#include <libxml/xmlreader.h>
#include <string>
#include <vector>
#include "./xmodel.hpp"

namespace flame { namespace io {

class IOXMLPop {
  public:
    IOXMLPop() {}
    int readXMLPop(std::string file_name, XModel * model);

  private:
    void processNode(xmlTextReaderPtr reader);

};
}}  // namespace flame::io
#endif  // IO__XML_POP_HPP_

