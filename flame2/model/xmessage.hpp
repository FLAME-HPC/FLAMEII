/*!
 * \file flame2/model/xmessage.hpp
 * \author Simon Coakley
 * \date 2012
 * \copyright Copyright (c) 2012 STFC Rutherford Appleton Laboratory
 * \copyright Copyright (c) 2012 University of Sheffield
 * \copyright GNU Lesser General Public License
 * \brief XMessage: holds message information
 */
#ifndef MODEL__XMESSAGE_HPP_
#define MODEL__XMESSAGE_HPP_
#include <boost/ptr_container/ptr_vector.hpp>
#include <string>
#include <vector>
#include "xvariable.hpp"
#include "task.hpp"

namespace flame { namespace model {

class XMessage {
  public:
    XMessage();
    void print();
    void setName(std::string name);
    std::string getName();
    XVariable * addVariable();
    XVariable * addVariable(std::string type, std::string name);
    boost::ptr_vector<XVariable> * getVariables();
    bool validateVariableName(std::string name);
    void setSyncStartTask(Task * task);
    Task * getSyncStartTask();
    void setSyncFinishTask(Task * task);
    Task * getSyncFinishTask();
    void setID(int id);
    int getID();

  private:
    int id_;
    std::string name_;
    boost::ptr_vector<XVariable> variables_;
    Task * syncStartTask_;
    Task * syncFinishTask_;
};
}}  // namespace flame::model
#endif  // MODEL__XMESSAGE_HPP_
