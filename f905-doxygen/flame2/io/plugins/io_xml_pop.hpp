/*!
 * \file flame2/io/plugins/io_xml_pop.hpp
 * \author Simon Coakley
 * \date 2012
 * \copyright Copyright (c) 2012 STFC Rutherford Appleton Laboratory
 * \copyright Copyright (c) 2012 University of Sheffield
 * \copyright GNU Lesser General Public License
 * \brief IOXMLPop: reading and writing of population XML file
 */
#ifndef IO__XML_POP_HPP_
#define IO__XML_POP_HPP_
#include <libxml/xmlreader.h>
#include <libxml/xmlwriter.h>
#include <boost/variant.hpp>
#include <string>
#include <vector>
#include <map>
#include <utility>
#include "xml_writer.hpp"
#include "../io_interface.hpp"

namespace flame { namespace io {

typedef std::pair<std::string, std::string> Var;

typedef std::pair<void*, size_t> PtrArray;
typedef std::map<std::string, PtrArray> VarPtrArrayMap;
typedef std::map<std::string, VarPtrArrayMap> AgentMemoryArrays;

typedef std::vector<Var> VarVec;
typedef std::map<std::string, VarVec> AgentMemory;

class IOXMLPop : public IO {
  public:
    IOXMLPop();
    std::string getName();
    void readPop(std::string file_name,
        void (*addInt)(std::string const&, std::string const&, int),
        void (*addDouble)(std::string const&, std::string const&, double));
    void writePop(std::string const& agent_name, std::string const& var_name,
        size_t size, void * ptr);
    void initialiseData();
    void finaliseData();
    void validateData(std::string const& data_file);

  private:
    AgentMemoryArrays agentMemoryArrays_;
    XMLWriter writer_;
    //! Pointer to method to add integers to flame2
    void (*addInt)(std::string const&, std::string const&, int);
    //! Pointer to method to add doubles to flame2
    void (*addDouble)(std::string const&, std::string const&, double);

    void openXMLDoc(xmlDocPtr * doc, std::string const& data_file);
    void writeAgents();
    void createDataSchemaHead();
    void createDataSchemaAgentNameType();
    void createDataSchemaAgentVarChoice();
    void createDataSchemaAgentVars();
    void createDataSchemaAgentVar(std::string type, std::string name);
    void createDataSchemaDefineAgents();
    void createDataSchemaDefineTags();
    void createDataSchema(xmlBufferPtr * buf);
    void processStartNode(std::vector<std::string> * tags, std::string name,
        xmlTextReaderPtr reader);
    void processTextVariable(std::string value, std::vector<std::string> * tags,
        std::string * agent);
    void processTextAgent(std::vector<std::string> * tags,
        xmlTextReaderPtr reader, std::string * agent);
    void processEndNode(std::vector<std::string> * tags, std::string name,
        std::string * agent);
    void processNode(xmlTextReaderPtr reader,
        std::vector<std::string> * tags, std::string * agent);
};
}}  // namespace flame::io
#endif  // IO__XML_POP_HPP_
