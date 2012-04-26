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
#include <boost/property_tree/ptree.hpp>
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
    int readUnknownElement(boost::property_tree::ptree::value_type const& v);
    int readIncludedModels(boost::property_tree::ptree::value_type const& v,
            std::string directory, model::XModel * model);
    int readIncludedModel(boost::property_tree::ptree::value_type const& v,
            std::string directory, model::XModel * model);
    int readEnvironment(boost::property_tree::ptree::value_type const& v,
            model::XModel * model);
    int readFunctionFiles(boost::property_tree::ptree::value_type const& v,
            model::XModel * model);
    int readDataTypes(boost::property_tree::ptree::value_type const& v,
            model::XModel * model);
    int readDataType(boost::property_tree::ptree::value_type const& v,
            model::XModel * model);
    int readTimeUnits(boost::property_tree::ptree::value_type const& v,
            model::XModel * model);
    int readTimeUnit(boost::property_tree::ptree::value_type const& v,
            model::XModel * model);
    int readVariables(boost::property_tree::ptree::value_type const& v,
            std::vector<model::XVariable*> * variables);
    int readVariable(boost::property_tree::ptree::value_type const& v,
            std::vector<model::XVariable*> * variables);
    int readAgents(boost::property_tree::ptree::value_type const& v,
            model::XModel * model);
    int readAgent(boost::property_tree::ptree::value_type const& v,
            model::XModel * model);
    int readTransitions(boost::property_tree::ptree::value_type const& v,
            model::XMachine * machine);
    int readTransition(boost::property_tree::ptree::value_type const& v,
            model::XMachine * machine);
    int readInputs(boost::property_tree::ptree::value_type const& v,
            model::XFunction * xfunction);
    int readInput(boost::property_tree::ptree::value_type const& v,
            model::XFunction * xfunction);
    int readOutputs(boost::property_tree::ptree::value_type const& v,
            model::XFunction * xfunction);
    int readOutput(boost::property_tree::ptree::value_type const& v,
            model::XFunction * xfunction);
    int readMessages(boost::property_tree::ptree::value_type const& v,
            model::XModel * model);
    int readMessage(boost::property_tree::ptree::value_type const& v,
            model::XModel * model);
    int readSort(boost::property_tree::ptree::value_type const& v,
            model::XIOput * xioput);
    int readCondition(boost::property_tree::ptree::value_type const& v,
            model::XCondition * xcondition);
};
}}}  // namespace flame::io::xml
#endif  // IO__XML_MODEL_HPP_
