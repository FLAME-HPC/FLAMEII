/*!
 * \file xparser2/xparser2.cpp
 * \author Shawn Chin, Simon Coakley
 * \date January 2013
 * \copyright Copyright (c) 2013 STFC Rutherford Appleton Laboratory
 * \copyright Copyright (c) 2013 University of Sheffield
 * \copyright GNU Lesser General Public License
 * \brief Xparser
 */
#include <iostream>
#include "flame2/model/xmodel.hpp"
#include "printer.hpp"
#include "codegen/gen_datastruct.hpp"
#include "xparser2.hpp"

int main(int argc, const char* argv[]) {
  flame::model::XModel model;  // just to check if we can access the module
  std::cout << "Parse me" << std::endl;

  xparser::Printer p(std::cout);
  xparser::codegen::GenDataStruct msg("location_message");
  msg.AddVar("double", "x");
  msg.AddVar("double", "y");
  msg.AddVar("int", "id");
  msg.Generate(p);

}
