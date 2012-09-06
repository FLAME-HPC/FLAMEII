/*!
 * \file src/model/xmodel_validate.hpp
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
#include "./xmachine.hpp"
#include "./xvariable.hpp"
#include "./xadt.hpp"
#include "./xtimeunit.hpp"
#include "./xmessage.hpp"

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
    int processVariables(std::vector<XVariable*> * variables_,
            XModel * model);
    int processAgentFunction(XFunction * function,
            std::vector<XVariable*> * variables);
    void validateVariableName(XVariable * v, int * errors,
            std::vector<XVariable*> * variables);
    void validateVariableType(XVariable * v, int * errors,
            XModel * model, bool allowDyamicArrays);
    int validateVariables(std::vector<XVariable*> * variables_,
            XModel * model, bool allowDyamicArrays);
    int validateFunctionFile(std::string name);
    int validateTimeUnitPeriod(XTimeUnit * timeUnit);
    int validateTimeUnitUnit(XTimeUnit * timeUnit, XModel * model);
    int validateTimeUnitName(XTimeUnit * timeUnit, XModel * model);
    int validateTimeUnit(XTimeUnit * timeUnit, XModel * model);
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
    int validateMessage(XMessage * xmessage, XModel * model);
    bool name_is_allowed(std::string name);
    int validateFunctionFiles(std::vector<std::string> names);
    int validateDataTypes(std::vector<XADT*> adts, XModel * model);
    int validateTimeUnits(std::vector<XTimeUnit*> timeUnits, XModel * model);
    int validateAgents(std::vector<XMachine*> agents, XModel * model);
    int validateMessages(std::vector<XMessage*> messages, XModel * model);
};
}}  // namespace flame::model
#endif  // MODEL__XMODEL_VALIDATE_HPP_
