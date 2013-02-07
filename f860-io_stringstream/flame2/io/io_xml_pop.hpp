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

namespace model = flame::model;

namespace flame { namespace io { namespace xml {

typedef std::vector<int>* intVecPtr;
typedef std::vector<double>* doubleVecPtr;
typedef std::map<std::string, std::vector<std::string> > agentVarMap;

typedef std::pair<std::string, std::string> Var;
typedef std::vector<Var> VarVec;
typedef std::map<std::string, VarVec> AgentMemory;

class IOXMLPop {
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
    void saveAgentVariableData(const AgentMemory& agentMemory);
    void writeAgents(xmlTextWriterPtr writer);
    void createDataSchemaHead(xmlTextWriterPtr writer);
    void createDataSchemaAgentNameType(xmlTextWriterPtr writer,
        const AgentMemory& agentMemory);
    void createDataSchemaAgentVarChoice(xmlTextWriterPtr writer,
        const AgentMemory& agentMemory);
    void createDataSchemaAgentVars(xmlTextWriterPtr writer,
        const AgentMemory& agentMemory);
    void createDataSchemaAgentVar(xmlTextWriterPtr writer,
        std::string type, std::string name);
    void createDataSchemaDefineAgents(xmlTextWriterPtr writer);
    void createDataSchemaDefineTags(xmlTextWriterPtr writer);
    void createDataSchema(xmlBufferPtr * buf,
        const AgentMemory& agentMemory);
    void writeXMLEndTag(xmlTextWriterPtr writer);
    void writeXMLEndTag(xmlTextWriterPtr writer, int count);
    void writeXMLTag(xmlTextWriterPtr writer, std::string name);
    void writeXMLTag(xmlTextWriterPtr writer, std::string name, int value);
    void writeXMLTag(xmlTextWriterPtr writer, std::string name, double value);
    void writeXMLTag(xmlTextWriterPtr writer,
        std::string name, std::string value);
    void writeXMLTagAttribute(xmlTextWriterPtr writer,
        std::string name, std::string value);
    void writeXMLTagAndAttribute(xmlTextWriterPtr writer, std::string tag,
        std::string name, std::string value);
    void writeXMLTagAndAttribute(xmlTextWriterPtr writer, std::string tag,
        std::string name1, std::string value1,
        std::string name2, std::string value2);
    void writeXMLTagAndAttribute(xmlTextWriterPtr writer, std::string tag,
        std::string name1, std::string value1,
        std::string name2, std::string value2,
        std::string name3, std::string value3);
    void endXMLDoc(xmlTextWriterPtr writer);
    void openXMLDoc(xmlDocPtr * doc, std::string const& data_file);
    int openXMLSchema(xmlSchemaValidCtxtPtr * valid_ctxt,
        xmlSchemaParserCtxtPtr * parser_ctxt,
        xmlSchemaPtr * schema, xmlBufferPtr * schema_buf);
    void processStartNode(std::vector<std::string> * tags, std::string name,
        xmlTextReaderPtr reader);
    template <class T>
    int processTextVariableCast(std::string value,
        std::vector<std::string> * tags,
        std::string * agent, xmlTextReaderPtr reader);
    int processTextVariable(std::string value, std::vector<std::string> * tags,
        std::string * agent, xmlTextReaderPtr reader,
        const AgentMemory& agentMemory);
    int processTextAgent(std::vector<std::string> * tags,
        xmlTextReaderPtr reader,
        std::string * agent, const AgentMemory& agentMemory);
    int processEndNode(std::vector<std::string> * tags, std::string name,
        std::string * agent);
    int processNode(xmlTextReaderPtr reader, const AgentMemory& agentMemory,
        std::vector<std::string> * tags, std::string * agent);
    std::string xml_pop_path;
    size_t iteration_;
    bool xml_pop_path_is_set;
    agentVarMap agentVarMap_;
};
}}}  // namespace flame::io::xml
#endif  // IO__XML_POP_HPP_
