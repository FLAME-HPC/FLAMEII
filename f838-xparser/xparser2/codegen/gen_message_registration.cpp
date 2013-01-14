/*!
 * \file xparser2/gen_message_registration.cpp
 * \author Shawn Chin
 * \date January 2013
 * \copyright Copyright (c) 2013 STFC Rutherford Appleton Laboratory
 * \copyright Copyright (c) 2013 University of Sheffield
 * \copyright GNU Lesser General Public License
 * \brief Generate code to define and register messages
 */
#include <string>
#include "gen_message_registration.hpp"
namespace xparser { namespace codegen {

GenMessageRegistration::GenMessageRegistration(const std::string& msg_name)
    : name_(msg_name) {
}

// var_type and var_name validation not performed
void GenMessageRegistration::AddVar(const std::string& var_type,
                                    const std::string& var_name) {
  vars_.push_back(VarPair(var_type, var_name));  // store type and var
}

void GenMessageRegistration::Generate(Printer* printer) const {
  // register message with model
  printer->Print("model.addMessage(\"$MSG$\");\n", "MSG", name_);

  // add vars to message
  VarPairVector::const_iterator i = vars_.begin();
  for (; i != vars_.end(); ++i) {
    printer->Print("model.addMessageVariable(\"$MSG$\", "
                   "\"$VARTYPE$\", \"$VARNAME$\");\n",
                   "MSG", name_,
                   "VARTYPE", i->first,
                   "VARNAME", i->second);
  }

  // register message datatype
  printer->Print("model.registerMessageType<$MSG$_message>(\"$MSG$\");\n",
                 "MSG", name_);
}

}}  // namespace xparser::codegen
