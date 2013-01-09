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
#include "../printer.hpp"
namespace xparser { namespace codegen {

class CodeGenerator {
  public:
    typedef xparser::Printer Printer;
    inline CodeGenerator() {}
    virtual ~CodeGenerator() {}
    virtual void Generate(Printer& printer) const = 0;
  private:
    CodeGenerator(const CodeGenerator&);
    void operator=(const CodeGenerator&);
};

}}  // namespace xparser::codegen

#endif  // XPARSER__CODEGEN__CODE_GENERATOR_HPP_
