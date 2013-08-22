/*!
 * \file xparser2/codegen/gen_message.hpp
 * \author Shawn Chin
 * \date January 2013
 * \copyright Copyright (c) 2013 STFC Rutherford Appleton Laboratory
 * \copyright Copyright (c) 2013 University of Sheffield
 * \copyright GNU Lesser General Public License
 * \brief Generate code to define messages
 */
#ifndef XPARSER__CODEGEN__GEN_MESSAGGE_HPP_
#define XPARSER__CODEGEN__GEN_MESSAGGE_HPP_
#include <vector>
#include <utility>
#include <string>
#include "code_generator.hpp"
namespace xparser { namespace codegen {

//! \brief Generates code to register a message and register its datatype
class GenMessage : public CodeGenerator {
  public:
    /*!
     * \brief Constructor
     * \param[in] msg_name Message name
     */
    explicit GenMessage(const std::string& msg_name);
    /*!
     * \brief Adds a message variable and its type
     * \param[in] var_type Variable type
     * \param[in] var_name Variable name
     */
    void AddVar(const std::string& var_type, const std::string& var_name);
    /*!
     * \brief Prints the generated code to the printer instance
     * \param[out] printer The printer to write to
     */
    void Generate(Printer* printer) const;

  private:
    //! \brief Type-name pair
    typedef std::pair<std::string, std::string> VarPair;
    //! \brief Collection of vars
    typedef std::vector<VarPair> VarPairVector;

    /*! \brief Collection of VarPair
     *
     * A Vector is used rather that a map so variables can be output in the
     * same order they were added
     */
    VarPairVector vars_;
    std::string name_;  //! message name
};
}}  // namespace xparser::codegen
#endif  // XPARSER__CODEGEN__GEN_MESSAGGES_HPP_
