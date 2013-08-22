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
#include "io_xml_agents.hpp"
#include "io_xml_variables.hpp"
#include "io_xml_element.hpp"

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
    IOXMLModel() : ioxmlagents(), ioxmlvariables(), ioxmlelement() {}
    /*!
     * \brief Reads a model XML file into an XModel
     * \param[in] file_name The path to the model file
     * \param[out] model Pointer to the XModel
     */
    void readXMLModel(std::string file_name, model::XModel * model);

  private:
    //! \brief Read agents
    IOXMLAgents ioxmlagents;
    //! \brief Read variables
    IOXMLVariables ioxmlvariables;
    //! \brief Read elements
    IOXMLElement ioxmlelement;
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
};
}}}  // namespace flame::io::xml
#endif  // IO__XML_MODEL_HPP_
