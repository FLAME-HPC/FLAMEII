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

namespace flame { namespace io {

class IOXMLModel {
  public:
    IOXMLModel() {}

    int readXMLModel(std::string file_name, XModel * model);

  private:
    int readUnknownElement(boost::property_tree::ptree::value_type const& v);
    int readEnvironment(boost::property_tree::ptree::value_type const& v);
    int readFunctionFiles(boost::property_tree::ptree::value_type const& v);
    int readVariables(boost::property_tree::ptree::value_type const& v,
            std::vector<XVariable*> * variables);
    int readVariable(boost::property_tree::ptree::value_type const& v,
            std::vector<XVariable*> * variables);
    int readAgents(boost::property_tree::ptree::value_type const& v,
            XModel * model);
    int readAgent(boost::property_tree::ptree::value_type const& v,
            XModel * model);
    int readTransitions(boost::property_tree::ptree::value_type const& v);
    int readTransition(boost::property_tree::ptree::value_type const& v);
    int readInputs(boost::property_tree::ptree::value_type const& v);
    int readInput(boost::property_tree::ptree::value_type const& v);
    int readOutputs(boost::property_tree::ptree::value_type const& v);
    int readOutput(boost::property_tree::ptree::value_type const& v);
    /*
     void readUnknownElement();
     void readModel();
     void readAgents();
     void readAgent();
    void readMessages();
    void readMessage();
     void readVariables(Machine * a);
     void readVariable(Machine * a);
     void readTransitions(Machine * a);
     void readTransition(Machine * a);
    void readCondition(Condition * c, QString * s);
    void readMpost();
    void readIncludedModels();
    void readIncludedModel();
     void readEnvironment();
    void readDataTypes(Machine * a);
    void readDataType(Machine * a);
    void readTimeUnits(Machine * a);
    void readTimeUnit(Machine * a);
     void readInputs(Communication * input);
     void readInput(Communication * input);
     void readOutputs(Communication * output);
     void readOutput(Communication * output);
    void readSort(MessageComm * input);
     void readFunctionFiles(Machine * a);
    */
};
}}  // namespace flame::io
#endif  // IO__XML_MODEL_HPP_
