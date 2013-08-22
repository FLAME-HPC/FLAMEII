/*!
 * \file xparser2/codegen/gen_message.cpp
 * \author Shawn Chin
 * \date January 2013
 * \copyright Copyright (c) 2013 STFC Rutherford Appleton Laboratory
 * \copyright Copyright (c) 2013 University of Sheffield
 * \copyright GNU Lesser General Public License
 * \brief Generate code to define messages
 */
#include <string>
#include "gen_message.hpp"
namespace xparser { namespace codegen {

GenMessage::GenMessage(const std::string& msg_name)
    : vars_(), name_(msg_name) {
}

// var_type and var_name validation not performed
void GenMessage::AddVar(const std::string& var_type,
                                    const std::string& var_name) {
  vars_.push_back(VarPair(var_type, var_name));  // store type and var
}

void GenMessage::Generate(Printer* printer) const {
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
}

}}  // namespace xparser::codegen
