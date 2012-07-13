/*!
 * \file src/model/xmodel.hpp
 * \author Simon Coakley
 * \date 2012
 * \copyright Copyright (c) 2012 STFC Rutherford Appleton Laboratory
 * \copyright Copyright (c) 2012 University of Sheffield
 * \copyright GNU Lesser General Public License
 * \brief XModel: holds model information
 */
#ifndef MODEL__XMODEL_HPP_
#define MODEL__XMODEL_HPP_
#include <string>
#include <vector>
#include "./xmachine.hpp"
#include "./xvariable.hpp"
#include "./xadt.hpp"
#include "./xtimeunit.hpp"
#include "./xmessage.hpp"

namespace flame { namespace model {

class XModel {
  public:
    XModel();
    ~XModel();
    int clear();
    void setup();
    void print();
    int validate();
    void setPath(std::string path);
    std::string getPath();
    void setName(std::string name);
    std::string getName();
    bool addIncludedModel(std::string name);
    std::vector<std::string> * getIncludedModels();
    XVariable * addConstant();
    std::vector<XVariable*> * getConstants();
    XADT * addADT();
    XADT * getADT(std::string name);
    std::vector<XADT*> * getADTs();
    XTimeUnit * addTimeUnit();
    std::vector<XTimeUnit*> * getTimeUnits();
    void addFunctionFile(std::string file);
    XMachine * addAgent();
    std::vector<XMachine*> * getAgents();
    XMachine * getAgent(std::string name);
    XMessage * addMessage();
    XMessage * getMessage(std::string name);
    std::vector<XMessage*> * getMessages();
    void addAllowedDataType(std::string name);
    std::vector<std::string> * getAllowedDataTypes();

  private:
    int processVariable(XVariable * variable,
            XModel * model);
    int processVariables(std::vector<XVariable*> * variables_,
            XModel * model);
    int processAgentFunction(XFunction * function,
            std::vector<XVariable*> * variables);
    void validateVariables_name(XVariable * v, int * errors,
            std::vector<XVariable*> * variables);
    int validateVariables(std::vector<XVariable*> * variables_,
            XModel * model, bool allowDyamicArrays);
    int validateFunctionFile(std::string name);
    int validateTimeunits(XTimeUnit * timeUnit, XModel * model);
    int validateADT(XADT * adt, XModel * model);
    int validateAgent(XMachine * agent, XModel * model);
    int validateAgentFunction(XFunction * xfunction,
            XMachine * agent, XModel * model);
    int validateAgentCommunication(XIOput * xioput, XMachine * agent,
            XModel * model);
    int validateAgentConditionOrFilter(XCondition * xcondition,
            XMachine * agent, XMessage * xmessage, XModel * model);
    int validateSort(XIOput * xioput, XMessage * xmessage);
    int validateMessage(XMessage * xmessage, XModel * model);
    bool name_is_allowed(std::string name);
    int validateFunctionFiles(std::vector<std::string> names);
    int validateDataTypes(std::vector<XADT*> adts, XModel * model);
    int validateTimeUnits(std::vector<XTimeUnit*> timeUnits, XModel * model);
    int validateAgents(std::vector<XMachine*> agents, XModel * model);
    int validateMessages(std::vector<XMessage*> messages, XModel * model);
    std::string name_;
    /*! \brief The absolute path to the model file */
    std::string path_;
    std::vector<std::string> includedModels_;
    std::vector<XVariable*> constants_;
    std::vector<XADT*> adts_;
    std::vector<XTimeUnit*> timeUnits_;
    std::vector<std::string> functionFiles_;
    std::vector<XMachine*> agents_;
    std::vector<XMessage*> messages_;
    /*! \brief A list of allowed data types to check variables */
    std::vector<std::string> allowedDataTypes_;
};
}}  // namespace flame::model
#endif  // MODEL__XMODEL_HPP_
