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
#include "./xmodel.hpp"
#include "./xmachine.hpp"
#include "./xcondition.hpp"

namespace flame { namespace io {

class IOXMLModel {
  public:
    IOXMLModel() {}

    int readXMLModel(std::string file_name, XModel * model);

  private:
    int readUnknownElement(boost::property_tree::ptree::value_type const& v);
    int readIncludedModels(boost::property_tree::ptree::value_type const& v,
            std::string directory, XModel * model);
    int readIncludedModel(boost::property_tree::ptree::value_type const& v,
            std::string directory, XModel * model);
    int readEnvironment(boost::property_tree::ptree::value_type const& v,
            XModel * model);
    int readFunctionFiles(boost::property_tree::ptree::value_type const& v,
            XModel * model);
    int readDataTypes(boost::property_tree::ptree::value_type const& v,
            XModel * model);
    int readDataType(boost::property_tree::ptree::value_type const& v,
            XModel * model);
    int readTimeUnits(boost::property_tree::ptree::value_type const& v,
            XModel * model);
    int readTimeUnit(boost::property_tree::ptree::value_type const& v,
            XModel * model);
    int readVariables(boost::property_tree::ptree::value_type const& v,
            std::vector<XVariable*> * variables);
    int readVariable(boost::property_tree::ptree::value_type const& v,
            std::vector<XVariable*> * variables);
    int readAgents(boost::property_tree::ptree::value_type const& v,
            XModel * model);
    int readAgent(boost::property_tree::ptree::value_type const& v,
            XModel * model);
    int readTransitions(boost::property_tree::ptree::value_type const& v,
            XMachine * machine);
    int readTransition(boost::property_tree::ptree::value_type const& v,
            XMachine * machine);
    int readInputs(boost::property_tree::ptree::value_type const& v,
            XFunction * xfunction);
    int readInput(boost::property_tree::ptree::value_type const& v,
            XFunction * xfunction);
    int readOutputs(boost::property_tree::ptree::value_type const& v,
            XFunction * xfunction);
    int readOutput(boost::property_tree::ptree::value_type const& v,
            XFunction * xfunction);
    int readMessages(boost::property_tree::ptree::value_type const& v,
            XModel * model);
    int readMessage(boost::property_tree::ptree::value_type const& v,
            XModel * model);
    int readSort(boost::property_tree::ptree::value_type const& v,
            XIOput * xioput);
    int readCondition(boost::property_tree::ptree::value_type const& v,
            XCondition * xcondition);
};
}}  // namespace flame::io
#endif  // IO__XML_MODEL_HPP_
