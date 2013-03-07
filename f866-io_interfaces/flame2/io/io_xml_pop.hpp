/*!
 * \file flame2/io/io_xml_pop.hpp
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
#include <libxml/xmlwriter.h>
#include <boost/variant.hpp>
#include <string>
#include <vector>
#include <map>
#include <utility>
#include "flame2/mem/memory_manager.hpp"
#include "flame2/model/xmodel.hpp"
#include "xml_writer.hpp"
#include "io_interface.hpp"

namespace model = flame::model;

namespace flame { namespace io {

typedef std::vector<int>* intVecPtr;
typedef std::vector<double>* doubleVecPtr;
typedef std::map<std::string, std::vector<std::string> > agentVarMap;

typedef std::pair<std::string, std::string> Var;
typedef std::vector<Var> VarVec;
typedef std::map<std::string, VarVec> AgentMemory;

class IOXMLPop : public IO {
  public:
    IOXMLPop();
    void readPop(std::string file_name, const AgentMemory& agentMemory);
    void writePop(std::string agent_name, std::string var_name);
    void initialiseData();
    void finaliseData();
    void validateData(std::string const& data_file,
                      const AgentMemory& agentMemory);
    bool xmlPopPathIsSet();
    std::string xmlPopPath();
    void setXmlPopPath(std::string path);
    void setIteration(size_t i);

  private:
    std::string xml_pop_path;
    size_t iteration_;
    bool xml_pop_path_is_set;
    agentVarMap agentVarMap_;
    XMLWriter writer_;

    void openXMLDoc(xmlDocPtr * doc, std::string const& data_file);
    void saveAgentVariableData(const AgentMemory& agentMemory);
    void writeAgents();
    void createDataSchemaHead();
    void createDataSchemaAgentNameType(const AgentMemory& agentMemory);
    void createDataSchemaAgentVarChoice(const AgentMemory& agentMemory);
    void createDataSchemaAgentVars(const AgentMemory& agentMemory);
    void createDataSchemaAgentVar(std::string type, std::string name);
    void createDataSchemaDefineAgents();
    void createDataSchemaDefineTags();
    void createDataSchema(xmlBufferPtr * buf,
        const AgentMemory& agentMemory);
    void processStartNode(std::vector<std::string> * tags, std::string name,
        xmlTextReaderPtr reader);
    template <class T>
    void processTextVariableCast(std::string value,
        std::vector<std::string> * tags,
        std::string * agent, xmlTextReaderPtr reader);
    void processTextVariable(std::string value, std::vector<std::string> * tags,
        std::string * agent, xmlTextReaderPtr reader,
        const AgentMemory& agentMemory);
    void processTextAgent(std::vector<std::string> * tags,
        xmlTextReaderPtr reader,
        std::string * agent, const AgentMemory& agentMemory);
    void processEndNode(std::vector<std::string> * tags, std::string name,
        std::string * agent);
    void processNode(xmlTextReaderPtr reader, const AgentMemory& agentMemory,
        std::vector<std::string> * tags, std::string * agent);
};
}}  // namespace flame::io
#endif  // IO__XML_POP_HPP_
