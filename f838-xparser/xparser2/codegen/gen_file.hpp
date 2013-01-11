/*!
 * \file xparser2/gen_file.hpp
 * \author Shawn Chin
 * \date January 2013
 * \copyright Copyright (c) 2013 STFC Rutherford Appleton Laboratory
 * \copyright Copyright (c) 2013 University of Sheffield
 * \copyright GNU Lesser General Public License
 * \brief abstract base class for generating files from a collection of
 *        generators
 */
#ifndef XPARSER__CODEGEN__GEN_FILE_HPP_
#define XPARSER__CODEGEN__GEN_FILE_HPP_
#include <boost/ptr_container/ptr_vector.hpp>
#include "code_generator.hpp"
namespace xparser { namespace codegen {

class GenFile : public CodeGenerator {
  public:
    void Generate(Printer& printer) const;

  protected:
    void GenerateIncludeStatements(Printer& printer) const;
};
}}  // namespace xparser::codegen
#endif  // XPARSER__CODEGEN__GEN_HEADERFILE_HPP_
