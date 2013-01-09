/*!
 * \file xparser2/file_generator.hpp
 * \author Shawn Chin
 * \date January 2013
 * \copyright Copyright (c) 2013 STFC Rutherford Appleton Laboratory
 * \copyright Copyright (c) 2013 University of Sheffield
 * \copyright GNU Lesser General Public License
 * \brief Handles the writing out of files based on input generators.
 */
#ifndef XPARSER__FILE_GENERATOR_HPP_
#define XPARSER__FILE_GENERATOR_HPP_
#include <string>
namespace xparser {

namespace codegen {
  class CodeGenerator;  // forward decl
}  // namespace xparser::codegen

class FileGenerator {
  public:
    // Use current working directory as output path
    FileGenerator();
    // Use custom output dir
    explicit FileGenerator(const std::string& output_dir);

    void ChangeOutputDirectory(const std::string& output_dir);
    
    // outfile should be a path relative to output_dir.
    // Target directory should already exist. If we do need to output files to
    // subdirectories, we could make this create directories that do not exist
    // (perhaps controlled by a flag).
    void Output(const std::string& outfile,
                const codegen::CodeGenerator& generator) const;
                
  private:
    std::string outdir_;

    
    
    FileGenerator(const FileGenerator&);
    void operator=(const FileGenerator&);
};

}  // namespace xparser
#endif  // XPARSER__FILE_GENERATOR_HPP_
