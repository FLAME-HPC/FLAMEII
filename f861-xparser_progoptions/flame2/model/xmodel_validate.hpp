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
    int validate();

  private:
    XModel * model;
    std::vector<std::string> * functionFiles_;
    boost::ptr_vector<XVariable> * constants_;
    boost::ptr_vector<XADT> * adts_;
    boost::ptr_vector<XTimeUnit> * timeUnits_;
    boost::ptr_vector<XMachine> * agents_;
    boost::ptr_vector<XMessage> * messages_;
    void processVariableDynamicArray(XVariable * variable);
    int processVariableStaticArray(XVariable * variable);
    int processVariable(XVariable * variable);
    int processVariables(boost::ptr_vector<XVariable> * variables_);
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
            bool allowDyamicArrays);
    int validateVariables(boost::ptr_vector<XVariable> * variables_,
            bool allowDyamicArrays);
    int validateFunctionFile(std::string name);
    int validateTimeUnit(XTimeUnit * timeUnit);
    int processTimeUnitPeriod(XTimeUnit * timeUnit);
    int processTimeUnitUnit(XTimeUnit * timeUnit);
    int processTimeUnit(XTimeUnit * timeUnit);
    int validateADT(XADT * adt);
    int validateAgent(XMachine * agent);
    int validateAgentStateGraph(XMachine * agent);
    int validateAgentFunctionIOput(XFunction * xfunction, XMachine * agent);
    int validateAgentFunction(XFunction * xfunction, XMachine * agent);
    int validateAgentCommunication(XIOput * xioput, XMachine * agent);
    int validateAgentConditionOrFilter(XCondition * xcondition,
            XMachine * agent, XMessage * xmessage);
    int validateSort(XIOput * xioput, XMessage * xmessage);
    int validateRandomString(XIOput * xioput);
    int validateMessage(XMessage * xmessage);
    bool name_is_allowed(std::string name);
    int validateTimeUnits(boost::ptr_vector<XTimeUnit> * timeUnits);
    int validateModelGraph();
};
}}  // namespace flame::model
#endif  // MODEL__XMODEL_VALIDATE_HPP_
