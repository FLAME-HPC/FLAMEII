/*!
 * \file xparser2/gen_datastruct.hpp
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

class GenDataStruct : public CodeGenerator {
  public:
    explicit GenDataStruct(const std::string& struct_name);
    void AddVar(const std::string& var_type, const std::string& var_name);
    void Generate(Printer* printer) const;
    
  private:
    typedef std::pair<std::string, std::string> VarPair;
    typedef std::vector<VarPair> VarPairVector;
    typedef std::set<std::string> VarnameSet;
    
    // store in vector rather than a map so vars can be output in the same
    // order they were added
    VarPairVector vars_;
    // Keep track of added vars to avoid dups
    VarnameSet dupe_check_;
    std::string struct_name_;
    
    void print_vars_(Printer* printer) const ;
    void print_stream_op_(Printer* printer) const ;
};

}}  // namespace xparser::codegen
#endif  // XPARSER__CODEGEN__CODE_GENERATOR_HPP_
