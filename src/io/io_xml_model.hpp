/*!
 * \file src/io/io_xml_model.hpp
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
#include "../model/model_manager.hpp"

namespace model = flame::model;

namespace flame { namespace io { namespace xml {

class IOXMLModel {
  public:
    IOXMLModel() {}

    int readXMLModel(std::string file_name, model::XModel * model);

  private:
    int readUnknownElement(xmlNode * node);
    int readIncludedModels(xmlNode * node,
            std::string directory, model::XModel * model);
    int readIncludedModel(xmlNode * node,
            std::string directory, model::XModel * model);
    int readEnvironment(xmlNode * node, model::XModel * model);
    int readFunctionFiles(xmlNode * node, model::XModel * model);
    int readDataTypes(xmlNode * node, model::XModel * model);
    int readDataType(xmlNode * node, model::XModel * model);
    int readTimeUnits(xmlNode * node, model::XModel * model);
    int readTimeUnit(xmlNode * node, model::XModel * model);
    int readVariables(xmlNode * node,
            std::vector<model::XVariable*> * variables);
    int readVariable(xmlNode * node,
            std::vector<model::XVariable*> * variables);
    int readAgents(xmlNode * node, model::XModel * model);
    int readAgent(xmlNode * node, model::XModel * model);
    int readTransitions(xmlNode * node, model::XMachine * machine);
    int readTransition(xmlNode * node, model::XMachine * machine);
    int readInputs(xmlNode * node, model::XFunction * xfunction);
    int readInput(xmlNode * node, model::XFunction * xfunction);
    int readOutputs(xmlNode * node, model::XFunction * xfunction);
    int readOutput(xmlNode * node, model::XFunction * xfunction);
    int readMessages(xmlNode * node, model::XModel * model);
    int readMessage(xmlNode * node, model::XModel * model);
    int readSort(xmlNode * node, model::XIOput * xioput);
    int readCondition(xmlNode * node, model::XCondition * xcondition);
};
}}}  // namespace flame::io::xml
#endif  // IO__XML_MODEL_HPP_
