/*!
 * \file xparser2/gen_datastruct.cpp
 * \author Shawn Chin
 * \date January 2013
 * \copyright Copyright (c) 2013 STFC Rutherford Appleton Laboratory
 * \copyright Copyright (c) 2013 University of Sheffield
 * \copyright GNU Lesser General Public License
 * \brief Generator for structs that can be store in flame::mem::VectorWrapper.
 *        To be used for messages and agent memory datatypes.
 */
#include <string>
#include <utility>
#include "flame2/exceptions/base.hpp"
#include "gen_datastruct.hpp"

namespace xparser { namespace codegen {

GenDataStruct::GenDataStruct(const std::string& struct_name)
    : struct_name_(struct_name) {
  RequireSysHeader("ostream");
}

// var_type and var_name validation not performed
void GenDataStruct::AddVar(const std::string& var_type,
                           const std::string& var_name) {
  if (dupe_check_.find(var_name) != dupe_check_.end()) {
    throw flame::exceptions::logic_error("variable already exists");
  }
  vars_.push_back(VarPair(var_type, var_name));  // store type and var
  dupe_check_.insert(var_name);  // remember var name for dupe check
}

void GenDataStruct::Generate(Printer* printer) const {
  // begin definition of struct
  printer->Print("typedef struct {\n");

  // write out vars
  printer->Indent();
  print_vars_(printer);
  printer->Outdent();

  // close definition
  printer->Print("} $SN$;\n\n", "SN", struct_name_);

  // overload operator<< for compatibility with flame::mem::VectorWrapper
  print_stream_op_(printer);
}

void GenDataStruct::print_vars_(Printer* printer) const {
  VarPairVector::const_iterator i = vars_.begin();
  for (; i != vars_.end(); ++i) {
    printer->Print("$TYPE$ $VAR$;\n", "TYPE", i->first, "VAR", i->second);
  }
}

void GenDataStruct::print_stream_op_(Printer* printer) const {
  // function definition
  printer->Print(
    "inline std::ostream &operator<<(std::ostream &os, const $SN$& ob) {\n",
    "SN", struct_name_);
  printer->Indent();

  // Output code that prints the struct in the form:
  //    struct_name {
  //      type1 var1 = value1;
  //      typr2 var2 = value2;
  //    }
  printer->Print("os << \"$SN$ {\" << std::endl;\n", "SN", struct_name_);
  VarPairVector::const_iterator i = vars_.begin();
  for (; i != vars_.end(); ++i) {
    printer->Print(
      "os << \"  $TYPE$ $VAR$ = \" << ob.$VAR$ << \";\" << std::endl;\n",
      "TYPE", i->first, "VAR", i->second);
  }
  printer->Print("os << \"}\" << std::endl;\n");
  printer->Print("return os;\n");

  // end function
  printer->Outdent();
  printer->Print("}\n");
}

}}  // namespace xparser::codegen
