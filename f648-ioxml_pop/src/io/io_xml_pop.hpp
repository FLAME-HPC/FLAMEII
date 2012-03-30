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
#include "../mem/memory_manager.hpp"

namespace flame { namespace io {

class IOXMLPop {
  public:
    IOXMLPop() {}
    int readXMLPop(std::string file_name, XModel * model,
            flame::mem::MemoryManager * memoryManager);
    int writeXMLPop(std::string file_name, int iterationNo,
            XModel * model,
            flame::mem::MemoryManager * memoryManager);

  private:
    int processNode(xmlTextReaderPtr reader,
            XModel * model,
            flame::mem::MemoryManager * memoryManager,
            std::vector<std::string> * tags,
            XMachine ** agent);
};
}}  // namespace flame::io
#endif  // IO__XML_POP_HPP_
