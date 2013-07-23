/*!
 * \file xparser2/codegen/gen_datastruct.hpp
 * \author Shawn Chin
 * \date January 2013
 * \copyright Copyright (c) 2013 STFC Rutherford Appleton Laboratory
 * \copyright Copyright (c) 2013 University of Sheffield
 * \copyright GNU Lesser General Public License
 * \brief Generator for structs that can be store in flame::mem::VectorWrapper.
 *        To be used for messages and agent memory datatypes.
 */
#ifndef XPARSER__CODEGEN__GEN_DATASTRUCT_HPP_
#define XPARSER__CODEGEN__GEN_DATASTRUCT_HPP_
#include <set>
#include <vector>
#include <utility>
#include <string>
#include "code_generator.hpp"
namespace xparser { namespace codegen {

//! \brief Generates struct declaration for datatypes that can be stored in
//! flame::mem::VectorWrapper
class GenDataStruct : public CodeGenerator {
  public:
    /*! \brief Constructor
     * \param struct_name Name of datatype to be declared in code output
     */
    explicit GenDataStruct(const std::string& struct_name);

    /*! \brief Adds a new variable to the target datatype
     * \param var_type Datatype of the variable
     * \param var_name Name of the variable
     *
     * Validation of datatypes and names is left to the caller.
     *
     * flame::exceptions::logic_error is thrown if a duplicate variable name
     * is added.
     */
    void AddVar(const std::string& var_type, const std::string& var_name);

    //! Writes generated code to printer instance
    void Generate(Printer* printer) const;

  private:
    typedef std::pair<std::string, std::string> VarPair;  //! type-name pair
    typedef std::vector<VarPair> VarPairVector;  //! Collection of VarPair
    typedef std::set<std::string> VarnameSet;  //! Collection of names

    /*! \brief Collection of VarPair
     *
     * A Vector is used rather that a map so variables can be output in the
     * same order they were added
     */
    VarPairVector vars_;
    VarnameSet dupe_check_;  //! Set of names used for detecting duplicate names
    std::string struct_name_;  //! Output struct name

    void print_vars_(Printer* printer) const;  //! Prints var declaration
    void print_stream_op_(Printer* printer) const;  //! Prints << method
};

}}  // namespace xparser::codegen
#endif  // XPARSER__CODEGEN__CODE_GENERATOR_HPP_
