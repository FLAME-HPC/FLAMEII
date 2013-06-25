/*!
 * \file flame2/io/io_xml_agent.hpp
 * \author Simon Coakley
 * \date 2012
 * \copyright Copyright (c) 2013 STFC Rutherford Appleton Laboratory
 * \copyright Copyright (c) 2013 University of Sheffield
 * \copyright GNU Lesser General Public License
 * \brief IOXMLAgents: reading of agents from XML
 */
#ifndef IO__IO_XML_AGENTS_HPP_
#define IO__IO_XML_AGENTS_HPP_
#include <libxml/parser.h>
#include <libxml/tree.h>
#include <string>
#include <vector>
#include "flame2/model/xmodel.hpp"
#include "io_xml_variables.hpp"
#include "io_xml_element.hpp"

namespace flame { namespace io { namespace xml {

class IOXMLAgents {
  public:
    IOXMLAgents() {}
    /*!
     * \brief Read agents
     * \param[in] node The element
     * \param[out] model The model
     */
    void readAgents(xmlNode * node, model::XModel * model);

  private:
    //! \brief Read variables
    IOXMLVariables ioxmlvariables;
    //! \brief Read elements
    IOXMLElement ioxmlelement;
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
     * \param[out] xconditiontime The condition time
     * \param[in] cur_node The element
     */
    void readConditionTime(
        model::XConditionTime * xconditiontime, xmlNode *cur_node);
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
#endif  // IO__XML_AGENTS_HPP_
