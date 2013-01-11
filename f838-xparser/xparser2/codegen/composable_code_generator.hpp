/*!
 * \file xparser2/gen_file.hpp
 * \author Shawn Chin
 * \date January 2013
 * \copyright Copyright (c) 2013 STFC Rutherford Appleton Laboratory
 * \copyright Copyright (c) 2013 University of Sheffield
 * \copyright GNU Lesser General Public License
 * \brief abstract base class CodeGenerators that can contain other generators
 */
#ifndef XPARSER__CODEGEN__COMPOSABLE_CODE_GENERATOR_HPP_
#define XPARSER__CODEGEN__COMPOSABLE_CODE_GENERATOR_HPP_
#include <boost/ptr_container/ptr_vector.hpp>
#include "code_generator.hpp"
namespace xparser { namespace codegen {

class ComposableCodeGenerator : public CodeGenerator {
  public:
    typedef boost::ptr_vector<CodeGenerator> GeneratorVector;

    template <typename T>
    inline void Insert(const T& generator) {
      // inherit header dependencies
      RequireHeader(generator.GetRequiredHeaders());
      RequireSysHeader(generator.GetRequiredSysHeaders());

      // Store copy of generator
      generators_.push_back(new T(generator));
    }

  protected:
    inline void GenerateInsertedContent(Printer& printer) const {
      GeneratorVector::const_iterator g;
      for (g = generators_.begin(); g != generators_.end(); ++g) {
        g->Generate(printer);
        printer.Print("\n");
      }
    }
    
  private:
    GeneratorVector generators_;
};
}} // namespace xparser::codegen
#endif  // XPARSER__CODEGEN__COMPOSABLE_CODE_GENERATOR_HPP_
