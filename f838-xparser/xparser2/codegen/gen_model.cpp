/*!
 * \file xparser2/gen_model.cpp
 * \author Simon Coakley
 * \date January 2013
 * \copyright Copyright (c) 2013 STFC Rutherford Appleton Laboratory
 * \copyright Copyright (c) 2013 University of Sheffield
 * \copyright GNU Lesser General Public License
 * \brief Generator for model registering.
 */
#include <string>
#include <utility>
#include "flame2/exceptions/base.hpp"
#include "gen_model.hpp"

namespace xparser { namespace codegen {

GenModel::GenModel() {}

void GenDataStruct::Generate(Printer& printer) const {
  // create model object
  printer.Print("model::Model model;\n");

  print_stream_op_(printer);
}

}}  // namespace xparser::codegen
