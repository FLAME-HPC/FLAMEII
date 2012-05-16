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
#include "../model/model_manager.hpp"
#include "../mem/memory_manager.hpp"

namespace model = flame::model;

namespace flame { namespace io { namespace xml {

class IOXMLPop {
  public:
    IOXMLPop() {}
    int readXMLPop(std::string file_name, model::XModel * model);
              // flame::mem::MemoryManager * memoryManager);
    int writeXMLPop(std::string file_name, int iterationNo,
            model::XModel * model);
              // flame::mem::MemoryManager * memoryManager);
    int createDataSchema(std::string const& file,
            flame::model::XModel * model);
    int validateData(std::string const& data_file,
            std::string const& schema_file);

  private:
    int processNode(xmlTextReaderPtr reader,
            model::XModel * model,
            // flame::mem::MemoryManager * memoryManager,
            std::vector<std::string> * tags,
            model::XMachine ** agent);
};
}}}  // namespace flame::io::xml
#endif  // IO__XML_POP_HPP_
