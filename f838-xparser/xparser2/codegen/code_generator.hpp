/*!
 * \file xparser2/code_generator.hpp
 * \author Shawn Chin
 * \date January 2013
 * \copyright Copyright (c) 2013 STFC Rutherford Appleton Laboratory
 * \copyright Copyright (c) 2013 University of Sheffield
 * \copyright GNU Lesser General Public License
 * \brief Abstract Base class for classes that generate code
 */
#ifndef XPARSER__CODEGEN__CODE_GENERATOR_HPP_
#define XPARSER__CODEGEN__CODE_GENERATOR_HPP_
namespace xparser {
  
class Printer;  // forward declaration

namespace codegen {

class CodeGenerator {
  public:
    inline CodeGenerator() {}
    virtual ~CodeGenerator() {}
    virtual void Generate(Printer& printer) const = 0;
  private:
    CodeGenerator(const CodeGenerator&);
    void operator=(const CodeGenerator&);
};

}  // namespace xparser::codegen
}  // namespace xparser
#endif  // XPARSER__CODEGEN__CODE_GENERATOR_HPP_
