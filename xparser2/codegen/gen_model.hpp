/*!
 * \file xparser2/codegen/gen_model.hpp
 * \author Simon Coakley
 * \date January 2013
 * \copyright Copyright (c) 2013 STFC Rutherford Appleton Laboratory
 * \copyright Copyright (c) 2013 University of Sheffield
 * \copyright GNU Lesser General Public License
 * \brief Generator for model registering.
 */
#ifndef XPARSER__CODEGEN__GEN_MODEL_HPP_
#define XPARSER__CODEGEN__GEN_MODEL_HPP_
#include <set>
#include <vector>
#include <utility>
#include <string>
#include "code_generator.hpp"

namespace xparser { namespace codegen {

//! \brief Generates code to register a model
class GenModel : public CodeGenerator {
  public:
    GenModel();
    //! Prints the generated code to the printer instance
    void Generate(Printer* printer) const;
};

}}  // namespace xparser::codegen
#endif  // XPARSER__CODEGEN__GENMODEL_HPP_
