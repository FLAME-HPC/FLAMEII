/*!
 * \file flame2/model/xfunction.hpp
 * \author Simon Coakley
 * \date 2012
 * \copyright Copyright (c) 2012 STFC Rutherford Appleton Laboratory
 * \copyright Copyright (c) 2012 University of Sheffield
 * \copyright GNU Lesser General Public License
 * \brief XFunction: holds transition function information
 */
#ifndef MODEL__XFUNCTION_HPP_
#define MODEL__XFUNCTION_HPP_
#include <string>
#include <vector>
#include "xioput.hpp"
#include "xcondition.hpp"
#include "xvariable.hpp"

namespace flame { namespace model {

class Task;

class XFunction {
  public:
    XFunction();
    explicit XFunction(std::string name);
    ~XFunction();
    void print();
    void setName(std::string name);
    std::string getName();
    void setCurrentState(std::string name);
    std::string getCurrentState();
    void setNextState(std::string name);
    std::string getNextState();
    XIOput * addInput();
    std::vector<XIOput*> * getInputs();
    XIOput * addOutput();
    std::vector<XIOput*> * getOutputs();
    XCondition * addCondition();
    XCondition * getCondition();
    void setMemoryAccessInfoAvailable(bool b);
    bool getMemoryAccessInfoAvailable();
    void addReadOnlyVariable(std::string name);
    std::vector<std::string> * getReadOnlyVariables();
    void addReadWriteVariable(std::string name);
    std::vector<std::string> * getReadWriteVariables();

  private:
    std::string name_;
    std::string currentState_;
    std::string nextState_;
    std::vector<XIOput*> inputs_;
    std::vector<XIOput*> outputs_;
    XCondition * condition_;
    /* Information on memory access */
    bool memoryAccessInfoAvailable_;
    std::vector<std::string> readOnlyVariables_;
    std::vector<std::string> readWriteVariables_;
};
}}  // namespace flame::model
#endif  // MODEL__XFUNCTION_HPP_
