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

class IOXMLModel {
  public:
    IOXMLModel() {}

    void readXMLModel(std::string file_name, model::XModel * model);

  private:
    void print_element_names(xmlNode * a_node);
    std::string getElementName(xmlNode * node);
    std::string getElementValue(xmlNode * node);
    void validateXMLModelRootElement(
            xmlNode *root_element, std::string file_name);
    void readModelElements(xmlNode *root_element, model::XModel * model,
            std::string directory);
    void readUnknownElement(xmlNode * node);
    void readIncludedModels(xmlNode * node,
            std::string directory, model::XModel * model);
    int readIncludedModel(xmlNode * node,
            std::string directory, model::XModel * model);
    int readIncludedModelValidate(std::string directory,
            std::string fileName, model::XModel * model, bool enable);
    int readEnvironment(xmlNode * node, model::XModel * model);
    int readFunctionFiles(xmlNode * node, model::XModel * model);
    int readDataTypes(xmlNode * node, model::XModel * model);
    int readDataType(xmlNode * node, model::XModel * model);
    int readTimeUnits(xmlNode * node, model::XModel * model);
    int readTimeUnit(xmlNode * node, model::XModel * model);
    int readVariables(xmlNode * node,
            boost::ptr_vector<model::XVariable> * variables);
    int readVariable(xmlNode * node,
            boost::ptr_vector<model::XVariable> * variables);
    int readAgents(xmlNode * node, model::XModel * model);
    int readAgent(xmlNode * node, model::XModel * model);
    void readTransitions(xmlNode * node, model::XMachine * machine);
    void readTransition(xmlNode * node, model::XMachine * machine);
    int readInputs(xmlNode * node, model::XFunction * xfunction);
    int readInput(xmlNode * node, model::XFunction * xfunction);
    int readOutputs(xmlNode * node, model::XFunction * xfunction);
    int readOutput(xmlNode * node, model::XFunction * xfunction);
    int readMessages(xmlNode * node, model::XModel * model);
    int readMessage(xmlNode * node, model::XModel * model);
    int readSort(xmlNode * node, model::XIOput * xioput);
    void readCondition(xmlNode * node, model::XCondition * xcondition);
    void readConditionTime(model::XCondition * xcondition, xmlNode *cur_node);
    void readConditionSide(model::XCondition * xcondition,
        model::XCondition ** hsCondition, std::string * hs, bool * hsIsValue,
        bool * hsIsCondition, xmlNode *cur_node);
    void readMemoryAccess(xmlNode * node, model::XFunction * xfunction);
    void readMemoryAccessVariables(xmlNode * node,
        std::vector<std::string> * variables);
};
}}}  // namespace flame::io::xml
#endif  // IO__XML_MODEL_HPP_
