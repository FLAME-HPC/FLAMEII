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

//! Data type to hold model variable type and name
typedef std::pair<std::string, std::string> Var;
//! Data type to hold an array pointer and its size
typedef std::pair<void*, size_t> PtrArray;
//! Data type to hold a map between variable names and PtrArray
typedef std::map<std::string, PtrArray> VarPtrArrayMap;
//! Data type to hold a map between agent names and VarPtrArrayMap
typedef std::map<std::string, VarPtrArrayMap> AgentMemoryArrays;
//! Data type to hold a vector of model variables
typedef std::vector<Var> VarVec;
//! Data type to hold a map between agent name and variables
typedef std::map<std::string, VarVec> AgentMemory;

/*!
 * \brief IO Plugin to read and write to XML files
 */
class IOXMLPop : public IO {
  public:
    //! Constructor
    IOXMLPop();
    /*!
     * \brief Return plugin name
     *
     * \return Plugin name
     */
    std::string getName();
    //! Read XML population file
    void readPop(std::string file_name,
        void (*addInt)(std::string const&, std::string const&, int),
        void (*addDouble)(std::string const&, std::string const&, double));
    //! Write agent variable array to XML file
    void writePop(std::string const& agent_name, std::string const& var_name,
        size_t size, void * ptr);
    //! Initialise XML file
    void initialiseData();
    //! Finalise XML file
    void finaliseData();
    //! Validate XML file with model definition
    void validateData(std::string const& data_file);

  private:
    //! Agent memory information
    AgentMemoryArrays agentMemoryArrays_;
    //! Helper class to write XML
    XMLWriter writer_;
    //! Pointer to method to add integers to flame2
    void (*addInt)(std::string const&, std::string const&, int);
    //! Pointer to method to add doubles to flame2
    void (*addDouble)(std::string const&, std::string const&, double);
    /*!
     * \brief Open an XML file and return an xml doc pointer
     * \return XML Doc pointer
     */
    void openXMLDoc(xmlDocPtr * doc, std::string const& data_file);
    //! Write agent data to file
    void writeAgents();
    //! Create data schema head
    void createDataSchemaHead();
    //! Create data schema agent name type
    void createDataSchemaAgentNameType();
    //! Create data schema agent var choice
    void createDataSchemaAgentVarChoice();
    //! Create data schema agent vars
    void createDataSchemaAgentVars();
    //! Create data schema agent var
    void createDataSchemaAgentVar(std::string type, std::string name);
    //! Create data schema agent definitions
    void createDataSchemaDefineAgents();
    //! Create data schema tag definitions
    void createDataSchemaDefineTags();
    //! Create data schema
    void createDataSchema(xmlBufferPtr * buf);
    //! Process xml tree start node
    void processStartNode(std::vector<std::string> * tags, std::string name,
        xmlTextReaderPtr reader);
    //! Process variable
    void processTextVariable(std::string value, std::vector<std::string> * tags,
        std::string * agent);
    //! Process agent
    void processTextAgent(std::vector<std::string> * tags,
        xmlTextReaderPtr reader, std::string * agent);
    //! Process xml tree end node
    void processEndNode(std::vector<std::string> * tags, std::string name,
        std::string * agent);
    //! Process xml tree node
    void processNode(xmlTextReaderPtr reader,
        std::vector<std::string> * tags, std::string * agent);
};
}}  // namespace flame::io
#endif  // IO__XML_POP_HPP_
