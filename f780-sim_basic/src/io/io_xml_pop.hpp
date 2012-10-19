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
#include <boost/variant.hpp>
#include <libxml/xmlreader.h>
#include <libxml/xmlwriter.h>
#include <string>
#include <vector>
#include "mem/memory_manager.hpp"
#include "model/xmodel.hpp"

namespace model = flame::model;

namespace flame { namespace io { namespace xml {

typedef std::vector<int>* intVecPtr;
typedef std::vector<double>* doubleVecPtr;
typedef std::map<std::string, std::vector<std::string> > agentVarMap;

class IOXMLPop {
  public:
    IOXMLPop();
    int readXMLPop(std::string file_name,
        model::XModel * model);
    int writeXMLPop(std::string file_name, int iterationNo,
        model::XModel * model);
    void writeXMLPop(std::string agent_name, std::string var_name);
    void initialiseData();
    void finaliseData();
    int createDataSchema(std::string const& file,
        flame::model::XModel * model);
    int validateData(std::string const& data_file,
        std::string const& schema_file);
    bool xmlPopPathIsSet();
    std::string xmlPopPath();
    void setXmlPopPath(std::string path);
    void setIteration(size_t i);

  private:
    int createDataSchemaHead(xmlTextWriterPtr writer);
    int createDataSchemaAgentNameType(xmlTextWriterPtr writer,
        flame::model::XModel * model);
    int createDataSchemaAgentVarChoice(xmlTextWriterPtr writer,
        flame::model::XModel * model);
    int createDataSchemaAgentVars(xmlTextWriterPtr writer,
        flame::model::XModel * model);
    int createDataSchemaAgentVar(xmlTextWriterPtr writer,
        std::vector<model::XVariable*>::iterator variable);
    int createDataSchemaDefineAgents(xmlTextWriterPtr writer);
    int createDataSchemaDefineTags(xmlTextWriterPtr writer);
    int writeXMLEndTag(xmlTextWriterPtr writer);
    int writeXMLEndTag(xmlTextWriterPtr writer, int count);
    int writeXMLTag(xmlTextWriterPtr writer, std::string name);
    int writeXMLTag(xmlTextWriterPtr writer, std::string name, int value);
    int writeXMLTag(xmlTextWriterPtr writer, std::string name, double value);
    int writeXMLTag(xmlTextWriterPtr writer,
        std::string name, std::string value);
    int writeXMLTagAttribute(xmlTextWriterPtr writer,
        std::string name, std::string value);
    int writeXMLTagAndAttribute(xmlTextWriterPtr writer, std::string tag,
        std::string name, std::string value);
    int writeXMLTagAndAttribute(xmlTextWriterPtr writer, std::string tag,
        std::string name1, std::string value1,
        std::string name2, std::string value2);
    int writeXMLTagAndAttribute(xmlTextWriterPtr writer, std::string tag,
        std::string name1, std::string value1,
        std::string name2, std::string value2,
        std::string name3, std::string value3);
    int endXMLDoc(xmlTextWriterPtr writer);
    template <class T>
    int setupVectorReader(model::XMachine * agent, model::XVariable * var,
        size_t * noAgents,
        std::vector< boost::variant<intVecPtr, doubleVecPtr> > * varVectors,
        size_t jj);
    int setupVectorReaders(model::XMachine * agent,
        size_t * noAgents,
        std::vector< boost::variant<intVecPtr, doubleVecPtr> > * varVectors);
    int writeXMLAgentVariables(model::XMachine * agent, xmlTextWriterPtr writer,
        std::vector< boost::variant<intVecPtr, doubleVecPtr> > * varVectors,
        size_t kk);
    int writeXMLAgent(model::XMachine * agent,
        xmlTextWriterPtr writer);
    int openXMLDoc(xmlDocPtr * doc, std::string const& data_file);
    int openXMLSchema(xmlSchemaValidCtxtPtr * valid_ctxt, std::string
        const& schema_file, xmlSchemaParserCtxtPtr * parser_ctxt,
        xmlSchemaPtr * schema, xmlDocPtr * schema_doc);
    int processStartNode(std::vector<std::string> * tags, std::string name);
    template <class T>
    int processTextVariableCast(std::string value,
        std::vector<std::string> * tags,
        model::XMachine ** agent);
    int processTextVariable(std::string value, std::vector<std::string> * tags,
        model::XMachine ** agent);
    int processTextAgent(std::vector<std::string> * tags,
        xmlTextReaderPtr reader,
        model::XMachine ** agent, model::XModel * model);
    int processEndNode(std::vector<std::string> * tags, std::string name,
        model::XMachine ** agent);
    int processNode(xmlTextReaderPtr reader, model::XModel * model,
        std::vector<std::string> * tags, model::XMachine ** agent);
    std::string xml_pop_path;
    size_t iteration_;
    bool xml_pop_path_is_set;
    agentVarMap agentVarMap_;
};
}}}  // namespace flame::io::xml
#endif  // IO__XML_POP_HPP_
