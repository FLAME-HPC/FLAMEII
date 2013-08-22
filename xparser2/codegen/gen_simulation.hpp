/*!
 * \file xparser2/codegen/gen_simulation.hpp
 * \author Shawn Chin
 * \author Simon Coakley
 * \date January 2013
 * \copyright Copyright (c) 2013 STFC Rutherford Appleton Laboratory
 * \copyright Copyright (c) 2013 University of Sheffield
 * \copyright GNU Lesser General Public License
 * \brief Generate code to setup simulation
 */
#ifndef XPARSER__CODEGEN__GEN_SIMULATION_HPP_
#define XPARSER__CODEGEN__GEN_SIMULATION_HPP_
#include <set>
#include <utility>
#include <string>
#include "code_generator.hpp"

namespace xparser { namespace codegen {

//! Generates code to setup a simulation
class GenSimulation : public CodeGenerator {
  public:
    //! \brief Set of message names
    typedef std::set<std::string> MessagenameSet;

    explicit GenSimulation();
    //! Adds a message type
    void AddMessage(const std::string& message_name);
    //! Prints the generated code to the printer instance
    void Generate(Printer* printer) const;

  private:
    //! Set of message names
    MessagenameSet messages_;
};
}}  // namespace xparser::codegen
#endif  // XPARSER__CODEGEN__GEN_SIMULATION_HPP_
