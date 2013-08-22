/*!
 * \file xparser2/codegen/gen_file.hpp
 * \author Shawn Chin
 * \date January 2013
 * \copyright Copyright (c) 2013 STFC Rutherford Appleton Laboratory
 * \copyright Copyright (c) 2013 University of Sheffield
 * \copyright GNU Lesser General Public License
 * \brief abstract base class for generating files from a collection of
 *        generators
 */
#ifndef XPARSER__CODEGEN__GEN_FILE_HPP_
#define XPARSER__CODEGEN__GEN_FILE_HPP_
#include <boost/ptr_container/ptr_vector.hpp>
#include "code_generator.hpp"
namespace xparser { namespace codegen {

/*! \brief Generates a file
 *
 * Prints out \c include statements for the header requirements of all
 * inserted generators. Then prints out the generated code for each inserted
 * generator (in the same order they were added).
 */
class GenFile : public CodeGenerator {
  public:
    //! /brief Constructor, initialise data members
    GenFile() : generators_() {}

    //! Writes generated file content to printer instance
    void Generate(Printer* printer) const;

    /*! \brief Appends sub-generator
     *  \param[in] generator The generator to insert
     *
     * A copy of the generator is store so the input generator can be modified
     * and reused.
     */
    template <typename T>
    inline void Insert(const T& generator) {
      // inherit header dependencies
      RequireHeader(generator.GetRequiredHeaders());
      RequireSysHeader(generator.GetRequiredSysHeaders());

      // Store copy of generator
      generators_.push_back(new T(generator));
    }

  protected:
    //! Prints include statements for all required headers
    void GenerateIncludeStatements(Printer* printer) const;
    //! Prints content produced by all sub-generators
    void GenerateInsertedContent(Printer* printer) const;

  private:
    typedef boost::ptr_vector<CodeGenerator> GeneratorVector;
    GeneratorVector generators_;  //! Collection of sub-generators
};
}}  // namespace xparser::codegen
#endif  // XPARSER__CODEGEN__GEN_HEADERFILE_HPP_
