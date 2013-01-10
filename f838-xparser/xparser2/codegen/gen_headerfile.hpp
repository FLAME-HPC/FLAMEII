/*!
 * \file xparser2/gen_headerfile.hpp
 * \author Shawn Chin
 * \date January 2013
 * \copyright Copyright (c) 2013 STFC Rutherford Appleton Laboratory
 * \copyright Copyright (c) 2013 University of Sheffield
 * \copyright GNU Lesser General Public License
 * \brief generic generator for building header files
 */
#ifndef XPARSER__CODEGEN__GEN_HEADERFILE_HPP_
#define XPARSER__CODEGEN__GEN_HEADERFILE_HPP_
#include <boost/ptr_container/ptr_vector.hpp>
#include "code_generator.hpp"
namespace xparser { namespace codegen {

class GenHeaderFile : public CodeGenerator {
  public:
    typedef boost::ptr_vector<CodeGenerator> GeneratorVector;
    
    GenHeaderFile() {}
    void Generate(Printer& printer) const;
    
    template <typename T>
    void Insert(const T& generator) {
      // inherit header dependencies
      RequireHeader(generator.GetRequiredHeaders());
      RequireSysHeader(generator.GetRequiredSysHeaders());

      // Store copy of generator
      generators_.push_back(new T(generator));
    }
    
  private:
    GeneratorVector generators_;
};
}}  // namespace xparser::codegen
#endif  // XPARSER__CODEGEN__GEN_HEADERFILE_HPP_
