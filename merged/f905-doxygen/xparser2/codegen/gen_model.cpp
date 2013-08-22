/*!
 * \file xparser2/codegen/gen_model.cpp
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

GenModel::GenModel() {
  // System headers used
  RequireHeader("flame2/model/model.hpp");
  RequireHeader("flame2/sim/simulation.hpp");
  // Generated model headers
  RequireHeader("message_datatypes.hpp");
  RequireHeader("agent_function_definitions.hpp");
}

void GenModel::Generate(Printer* printer) const {
  // create model object
  printer->Print("flame::model::Model model;\n");
  // handle model environment stuff here
  // ...
}

}}  // namespace xparser::codegen
