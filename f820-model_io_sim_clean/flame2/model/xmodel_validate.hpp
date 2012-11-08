/*!
 * \file flame2/model/xmodel_validate.hpp
 * \author Simon Coakley
 * \date 2012
 * \copyright Copyright (c) 2012 STFC Rutherford Appleton Laboratory
 * \copyright Copyright (c) 2012 University of Sheffield
 * \copyright GNU Lesser General Public License
 * \brief XModelValidate: holds model validator
 */
#ifndef MODEL__XMODEL_VALIDATE_HPP_
#define MODEL__XMODEL_VALIDATE_HPP_
#include <string>
#include <vector>
#include <set>
#include "xmachine.hpp"
#include "xvariable.hpp"
#include "xadt.hpp"
#include "xtimeunit.hpp"
#include "xmessage.hpp"

namespace flame { namespace model {

class XModelValidate {
  public:
    explicit XModelValidate(XModel * m);
    ~XModelValidate();
    int validate();

  private:
    XModel * model;
    void processVariableDynamicArray(XVariable * variable);
    int processVariableStaticArray(XVariable * variable);
    int processVariable(XVariable * variable,
            XModel * model);
    int processVariables(boost::ptr_vector<XVariable> * variables_,
            XModel * model);
    bool variableExists(std::string name,
            boost::ptr_vector<XVariable> * variables);
    int processMemoryAccessVariable(std::string name,
            boost::ptr_vector<XVariable> * variables,
            std::set<std::string> * usedVariables);
    int processAgentFunction(XFunction * function,
            boost::ptr_vector<XVariable> * variables);
    void validateVariableName(XVariable * v, int * errors,
            boost::ptr_vector<XVariable> * variables);
    void validateVariableType(XVariable * v, int * errors,
            XModel * model, bool allowDyamicArrays);
    int validateVariables(boost::ptr_vector<XVariable> * variables_,
            XModel * model, bool allowDyamicArrays);
    int validateFunctionFile(std::string name);
    int validateTimeUnit(XTimeUnit * timeUnit, XModel * model);
    int processTimeUnitPeriod(XTimeUnit * timeUnit);
    int processTimeUnitUnit(XTimeUnit * timeUnit, XModel * model);
    int processTimeUnit(XTimeUnit * timeUnit);
    int validateADT(XADT * adt, XModel * model);
    int validateAgent(XMachine * agent, XModel * model);
    int validateAgentStateGraph(XMachine * agent);
    int validateAgentFunctionIOput(XFunction * xfunction, XMachine * agent,
            XModel * model);
    int validateAgentFunction(XFunction * xfunction,
            XMachine * agent, XModel * model);
    int validateAgentCommunication(XIOput * xioput, XMachine * agent,
            XModel * model);
    int validateAgentConditionOrFilter(XCondition * xcondition,
            XMachine * agent, XMessage * xmessage, XModel * model);
    int validateSort(XIOput * xioput, XMessage * xmessage);
    int validateRandomString(XIOput * xioput);
    int validateMessage(XMessage * xmessage, XModel * model);
    bool name_is_allowed(std::string name);
    int validateFunctionFiles(std::vector<std::string> * names);
    int validateDataTypes(boost::ptr_vector<XADT> * adts, XModel * model);
    int validateTimeUnits(
            boost::ptr_vector<XTimeUnit> * timeUnits, XModel * model);
    int validateAgents(boost::ptr_vector<XMachine> * agents, XModel * model);
    int validateMessages(
            boost::ptr_vector<XMessage> * messages, XModel * model);
};
}}  // namespace flame::model
#endif  // MODEL__XMODEL_VALIDATE_HPP_
