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
