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

//! \brief Data type to hold model variable type and name
typedef std::pair<std::string, std::string> Var;
//! \brief Data type to hold an array pointer and its size
typedef std::pair<void*, size_t> PtrArray;
//! \brief Data type to hold a map between variable names and PtrArray
typedef std::map<std::string, PtrArray> VarPtrArrayMap;
//! \brief Data type to hold a map between agent names and VarPtrArrayMap
typedef std::map<std::string, VarPtrArrayMap> AgentMemoryArrays;
//! \brief Data type to hold a vector of model variables
typedef std::vector<Var> VarVec;
//! \brief Data type to hold a map between agent name and variables
typedef std::map<std::string, VarVec> AgentMemory;

/*!
 * \brief IO Plugin to read and write to XML files
 */
class IOXMLPop : public IOInterface {
  public:
    /*!
     * \brief Constructor
     */
    IOXMLPop();
    /*!
     * \brief Return plugin name
     * \return Plugin name
     */
    std::string getName();
    /*!
     * \brief Read XML population file
     * \param[in] file_name File to read
     * \param[out] addInt Function to add integers
     * \param[out] addDouble Function to add doubles
     */
    void readPop(std::string file_name,
        void (*addInt)(std::string const&, std::string const&, int),
        void (*addDouble)(std::string const&, std::string const&, double));
    /*!
     * \brief Write agent variable array to XML file
     * \param[in] agent_name The agent name
     * \param[in] var_name The variable name
     * \param[in] size The size of the variable array
     * \param[in] ptr Pointer to the variable array
     */
    void writePop(std::string const& agent_name, std::string const& var_name,
        size_t size, void * ptr);
    /*!
     * \brief Initialise XML file
     */
    void initialiseData();
    /*!
     * \brief Finalise XML file
     */
    void finaliseData();
    /*!
     * \brief Validate XML file with model definition
     * \param[in] data_file
     */
    void validateData(std::string const& data_file);

  private:
    //! \brief Agent memory information
    AgentMemoryArrays agentMemoryArrays_;
    //! \brief Helper class to write XML
    XMLWriter writer_;
    /*!
     * \brief Pointer to method to add integers to flame2
     * \param[in] Variable type
     * \param[in] Variable name
     * \param[in] Integer value
     */
    void (*addInt)(std::string const&, std::string const&, int);
    /*!
     * \brief Pointer to method to add doubles to flame2
     * \param[in] Variable type
     * \param[in] Variable name
     * \param[in] Double value
     */
    void (*addDouble)(std::string const&, std::string const&, double);
    /*!
     * \brief Open an XML file and return an xml doc pointer
     * \param[out] doc The opened doc
     * \param[in] data_file The file to open
     */
    void openXMLDoc(xmlDocPtr * doc, std::string const& data_file);
    /*!
     * \brief Write agent data to file
     */
    void writeAgents();
    /*!
     * \brief Create data schema head
     */
    void createDataSchemaHead();
    /*!
     * \brief Create data schema agent name type
     */
    void createDataSchemaAgentNameType();
    /*!
     * \brief Create data schema agent var choice
     */
    void createDataSchemaAgentVarChoice();
    /*!
     * \brief Create data schema agent vars
     */
    void createDataSchemaAgentVars();
    /*!
     * \brief Create data schema agent var
     * \param[in] type The variable type
     * \param[in] name The variable name
     */
    void createDataSchemaAgentVar(std::string type, std::string name);
    /*!
     * \brief Create data schema agent definitions
     */
    void createDataSchemaDefineAgents();
    /*!
     * \brief Create data schema tag definitions
     */
    void createDataSchemaDefineTags();
    /*!
     * \brief Create data schema
     * \param[in,out] buf XML buffer to write to
     */
    void createDataSchema(xmlBufferPtr * buf);
    /*!
     * \brief Process xml tree start node
     * \param[in,out] tags Read XML tags
     * \param[in] name Tag name
     * \param[in] reader XML reader
     */
    void processStartNode(std::vector<std::string> * tags, std::string name,
        xmlTextReaderPtr reader);
    /*!
     * \brief Process variable
     * \param[in] value The variable value
     * \param[in,out] tags Read XML tags
     * \param[in] agent The variables parent agent
     */
    void processTextVariable(std::string value, std::vector<std::string> * tags,
        std::string * agent);
    /*!
     * \brief Process agent
     * \param[in,out] tags Read XML tags
     * \param[in] reader XML reader
     * \param[in] agent The agent
     */
    void processTextAgent(std::vector<std::string> * tags,
        xmlTextReaderPtr reader, std::string * agent);
    /*!
     * \brief Process xml tree end node
     * \param[in,out] tags Read XML tags
     * \param[in] name Tag name
     * \param[in] agent The agent
     */
    void processEndNode(std::vector<std::string> * tags, std::string name,
        std::string * agent);
    /*!
     * \brief Process xml tree node
     * \param[in] reader XML reader
     * \param[in,out] tags Read XML tags
     * \param[in] agent The agent
     */
    void processNode(xmlTextReaderPtr reader,
        std::vector<std::string> * tags, std::string * agent);
};
}}  // namespace flame::io
#endif  // IO__XML_POP_HPP_
