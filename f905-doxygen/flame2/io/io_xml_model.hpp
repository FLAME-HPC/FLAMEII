/*!
 * \file flame2/io/io_xml_model.hpp
 * \author Simon Coakley
 * \date 2012
 * \copyright Copyright (c) 2012 STFC Rutherford Appleton Laboratory
 * \copyright Copyright (c) 2012 University of Sheffield
 * \copyright GNU Lesser General Public License
 * \brief IOXMLModel: reading of model XML file
 */
#ifndef IO__XML_MODEL_HPP_
#define IO__XML_MODEL_HPP_
#include <libxml/parser.h>
#include <libxml/tree.h>
#include <string>
#include <vector>
#include "flame2/model/xmodel.hpp"

namespace model = flame::model;

namespace flame { namespace io { namespace xml {

/*!
 * \brief Reads model XML files
 */
class IOXMLModel {
  public:
    /*!
     * \brief Constructor
     */
    IOXMLModel() {}
    /*!
     * \brief Reads a model XML file into an XModel
     * \param[in] file_name The path to the model file
     * \param[out] model Pointer to the XModel
     */
    void readXMLModel(std::string file_name, model::XModel * model);

  private:
    /*!
     * \brief Print XML element names
     * \param[in] a_node The element
     */
    void print_element_names(xmlNode * a_node);
    /*!
     * \brief Get element name
     * \param[in] node The element
     * \return The element name
     */
    std::string getElementName(xmlNode * node);
    /*!
     * \brief Get element value
     * \param[in] node The element
     * \return The element value
     */
    std::string getElementValue(xmlNode * node);
    /*!
     * \brief Validate XML root element
     * \param[in] root_element The root element
     * \param[in] file_name The file name
     * \param[in] doc The XML doc
     */
    void validateXMLModelRootElement(
        xmlNode *root_element, std::string file_name, xmlDoc *doc);
    /*!
     * \brief Read model elements
     * \param[in] root_element The root element
     * \param[out] model The model
     * \param[in] directory The directory of the model file
     * \param[in] doc The XML doc
     */
    void readModelElements(xmlNode *root_element, model::XModel * model,
        std::string directory, xmlDoc *doc);
    /*!
     * \brief Read unknown element
     * \param[in] node The element
     */
    void readUnknownElement(xmlNode * node);
    /*!
     * \brief Read included models
     * \param[in] node The element
     * \param[in] directory The directory of the model file
     * \param[out] model The model
     * \param[in] doc The XML doc
     */
    void readIncludedModels(xmlNode * node,
        std::string directory, model::XModel * model, xmlDoc *doc);
    /*!
     * \brief Read included model
     * \param[in] node The element
     * \param[in] directory The directory of the model file
     * \param[out] model The model
     * \param[in] doc The XML doc
     */
    void readIncludedModel(xmlNode * node,
        std::string directory, model::XModel * model, xmlDoc *doc);
    /*!
     * \brief Validate included model
     * \param[in] directory The directory of the model file
     * \param[in] fileName The model file name
     * \param[out] model The model
     * \param[out] enable If the included model is enabled
     * \param[in] doc The XML doc
     */
    void readIncludedModelValidate(std::string directory, std::string fileName,
        model::XModel * model, bool enable, xmlDoc *doc);
    /*!
     * \brief Read environment
     * \param[in] node The element
     * \param[out] model The model
     */
    void readEnvironment(xmlNode * node, model::XModel * model);
    /*!
     * \brief Read function files
     * \param[in] node The element
     * \param[out] model The model
     */
    void readFunctionFiles(xmlNode * node, model::XModel * model);
    /*!
     * \brief Read data types
     * \param[in] node The element
     * \param[out] model The model
     */
    void readDataTypes(xmlNode * node, model::XModel * model);
    /*!
     * \brief Read data type
     * \param[in] node The element
     * \param[out] model The model
     */
    void readDataType(xmlNode * node, model::XModel * model);
    /*!
     * \brief Read time units
     * \param[in] node The element
     * \param[out] model The model
     */
    void readTimeUnits(xmlNode * node, model::XModel * model);
    /*!
     * \brief read time unit
     * \param[in] node The element
     * \param[out] model The model
     */
    void readTimeUnit(xmlNode * node, model::XModel * model);
    /*!
     * \brief Read variables
     * \param[in] node The element
     * \param[out] variables The variables
     */
    void readVariables(xmlNode * node,
        boost::ptr_vector<model::XVariable> * variables);
    /*!
     * \brief Read a variable
     * \param[in] node The element
     * \param[out] variables The variables
     */
    void readVariable(xmlNode * node,
        boost::ptr_vector<model::XVariable> * variables);
    /*!
     * \brief Read agents
     * \param[in] node The element
     * \param[out] model The model
     */
    void readAgents(xmlNode * node, model::XModel * model);
    /*!
     * \brief Read an agent
     * \param[in] node The element
     * \param[out] model The model
     */
    void readAgent(xmlNode * node, model::XModel * model);
    /*!
     * \brief Read agent transitions
     * \param[in] node The element
     * \param[out] machine The agent
     */
    void readTransitions(xmlNode * node, model::XMachine * machine);
    /*!
     * \brief Read agent transition
     * \param[in] node The element
     * \param[out] machine The agent
     */
    void readTransition(xmlNode * node, model::XMachine * machine);
    /*!
     * \brief Read function inputs
     * \param[in] node The element
     * \param[out] xfunction The function
     */
    void readInputs(xmlNode * node, model::XFunction * xfunction);
    /*!
     * \brief Read function input
     * \param[in] node The element
     * \param[out] xfunction The function
     */
    void readInput(xmlNode * node, model::XFunction * xfunction);
    /*!
     * \brief Read function outputs
     * \param[in] node The element
     * \param[out] xfunction The function
     */
    void readOutputs(xmlNode * node, model::XFunction * xfunction);
    /*!
     * \brief Read function output
     * \param[in] node The element
     * \param[out] xfunction The function
     */
    void readOutput(xmlNode * node, model::XFunction * xfunction);
    /*!
     * \brief Read messages
     * \param[in] node The element
     * \param[out] model The model
     */
    void readMessages(xmlNode * node, model::XModel * model);
    /*!
     * \brief Read message
     * \param[in] node The element
     * \param[out] model The model
     */
    void readMessage(xmlNode * node, model::XModel * model);
    /*!
     * \brief Read input sort
     * \param[in] node The element
     * \param[out] xioput The input
     */
    void readSort(xmlNode * node, model::XIOput * xioput);
    /*!
     * \brief Read a condition
     * \param[in] node The element
     * \param[out] xcondition The condition
     */
    void readCondition(xmlNode * node, model::XCondition * xcondition);
    /*!
     * \brief Read condition time
     * \param[out] xcondition The condition
     * \param[in] cur_node The element
     */
    void readConditionTime(model::XCondition * xcondition, xmlNode *cur_node);
    /*!
     * \brief Read condition left hand side
     * \param[out] xcondition The condition
     * \param[in] cur_node The element
     */
    void readConditionLhs(model::XCondition * xcondition,
        xmlNode *cur_node);
    /*!
     * \brief Read condition right hand side
     * \param[out] xcondition The condition
     * \param[in] cur_node The element
     */
    void readConditionRhs(model::XCondition * xcondition,
        xmlNode *cur_node);
    /*!
     * \brief Read function memory access
     * \param[in] node The element
     * \param[out] xfunction The function
     */
    void readMemoryAccess(xmlNode * node, model::XFunction * xfunction);
    /*!
     * \brief Read memory access variables
     * \param[in] node The element
     * \param[out] variables The memory access variables
     */
    void readMemoryAccessVariables(xmlNode * node,
        std::vector<std::string> * variables);
};
}}}  // namespace flame::io::xml
#endif  // IO__XML_MODEL_HPP_
