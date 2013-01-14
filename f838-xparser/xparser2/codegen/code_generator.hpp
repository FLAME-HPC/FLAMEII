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
#include <set>
#include <string>
#include <iostream>
#include "../printer.hpp"
namespace xparser { namespace codegen {

class CodeGenerator {
  public:
    typedef xparser::Printer Printer;
    typedef std::set<std::string> StringSet;

    inline CodeGenerator() {}
    virtual ~CodeGenerator() {}
    virtual void Generate(Printer* printer) const = 0;

    inline const StringSet& GetRequiredSysHeaders() const {
      return required_sys_headers_;
    }

    inline const StringSet& GetRequiredHeaders() const {
      return required_headers_;
    }

  protected:
    inline void RequireHeader(const std::string& header) {
      required_headers_.insert(header);
    }

    inline void RequireHeader(const StringSet& header) {
      required_headers_.insert(header.begin(), header.end());
    }

    inline void RequireSysHeader(const std::string& header) {
      required_sys_headers_.insert(header);
    }

    inline void RequireSysHeader(const StringSet& header) {
      required_sys_headers_.insert(header.begin(), header.end());
    }

  private:
    StringSet required_headers_;
    StringSet required_sys_headers_;
    
};

}}  // namespace xparser::codegen

#endif  // XPARSER__CODEGEN__CODE_GENERATOR_HPP_
