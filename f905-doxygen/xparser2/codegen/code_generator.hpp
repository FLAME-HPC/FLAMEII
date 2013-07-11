/*!
 * \file xparser2/codegen/code_generator.hpp
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

//! Abstract base class for generator objects
class CodeGenerator {
  public:
    //! \brief Shortcut to Printer class
    typedef xparser::Printer Printer;
    //! \brief Shortcut to set of strings
    typedef std::set<std::string> StringSet;

    /*! \brief Constructor
     *
     * Subclasses should overload this to make calls to RequireHeader() and
     * RequireSysHeader() when required.
     */
    inline CodeGenerator() : required_headers_(), required_sys_headers_() {}

    //! Destructor
    virtual ~CodeGenerator() {}

    /*! \brief Writes generated output to given Printer instance
     *  \param[out] printer Used to write output to
     *
     * Subclasses must overload this to perform the actual code generation
     * and printing.
     */
    virtual void Generate(Printer* printer) const = 0;

    //! Returns a set of required system headers
    inline const StringSet& GetRequiredSysHeaders() const {
      return required_sys_headers_;
    }

    //! Returns a set of required headers
    inline const StringSet& GetRequiredHeaders() const {
      return required_headers_;
    }

  protected:
    /*! \brief Specifies a header required by the generated code
     *  \param[in] header The required header
     *
     * This method should be called in the construct if required
     */
    inline void RequireHeader(const std::string& header) {
      required_headers_.insert(header);
    }

    /*! \brief Specifies a header required by the generated code
     *  \param[in] header Set of required headers
     *
     * This method should be called in the construct if required
     */
    inline void RequireHeader(const StringSet& header) {
      required_headers_.insert(header.begin(), header.end());
    }

    /*! \brief Specifies a system header required by the generated code
     *  \param[in] header The required system header
     *
     * This method should be called in the construct if required
     */
    inline void RequireSysHeader(const std::string& header) {
      required_sys_headers_.insert(header);
    }

    /*! \brief Specifies a system header required by the generated code
     *  \param[in] header Set of required system headers
     *
     * This method should be called in the construct if required
     */
    inline void RequireSysHeader(const StringSet& header) {
      required_sys_headers_.insert(header.begin(), header.end());
    }

  private:
    StringSet required_headers_;  //! collection of required headers
    StringSet required_sys_headers_;  //! collection of required system headers
};

}}  // namespace xparser::codegen

#endif  // XPARSER__CODEGEN__CODE_GENERATOR_HPP_
