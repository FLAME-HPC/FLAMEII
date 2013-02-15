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

//! \brief Manages the writing of generated code to files
class FileGenerator {
  public:
    /*! \brief Default constructor
     *
     * The output directory is set to the working directory of the calling
     * binary.
     */
    FileGenerator();

    /*! \brief Alternative constructor
     * \param output_dir Output directory for all generated files
     *
     * Sets the output directory to the given directory.
     * Throws flame::exceptions::invalid_argument if the given directory is
     * not valid.
     */
    explicit FileGenerator(const std::string& output_dir);

    /*! \brief Changes the output directory
     * \param output_dir New output directory
     *
     * Changes the output directory to the given directory.
     * Throws flame::exceptions::invalid_argument if the given directory is
     * not valid.
     */
    void ChangeOutputDirectory(const std::string& output_dir);

    // outfile should be a path relative to output_dir.
    // Target directory should already exist. If we do need to output files to
    // subdirectories, we could make this create directories that do not exist
    // (perhaps controlled by a flag).
    /*! \brief Write output file using the given code generator
     * \param outfile file path relative to output diretory
     * \param generator Code generator instance used to produce file content
     *
     * If \c outfile is prefixed with a subdirectory, it is the caller's
     * responsibility to ensure that the directory exists.
     *
     * If the file exists, it will be overwritten.
     *
     * Throws flame::exceptions::invalid_argument if the file cannot be
     * written to.
     */
    void Output(const std::string& outfile,
                const codegen::CodeGenerator& generator) const;

  private:
    std::string outdir_;  //! Output directory

    // disable copy constructor and assignment
    FileGenerator(const FileGenerator&);
    void operator=(const FileGenerator&);
};

}  // namespace xparser
#endif  // XPARSER__FILE_GENERATOR_HPP_
