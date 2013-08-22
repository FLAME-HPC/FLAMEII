/*!
 * \file xparser2/file_generator.cpp
 * \author Shawn Chin
 * \date January 2013
 * \copyright Copyright (c) 2013 STFC Rutherford Appleton Laboratory
 * \copyright Copyright (c) 2013 University of Sheffield
 * \copyright GNU Lesser General Public License
 * \brief Handles the writing out of files based on input generators.
 */
#include <fstream>
#include <string>
#include <boost/filesystem.hpp>
#include "flame2/exceptions/base.hpp"
#include "codegen/code_generator.hpp"
#include "printer.hpp"
#include "file_generator.hpp"
namespace xparser {

FileGenerator::FileGenerator() : outdir_() {
  // use current working dir as output dir
  ChangeOutputDirectory(boost::filesystem::current_path().string());
}

FileGenerator::FileGenerator(const std::string& output_dir) : outdir_() {
  ChangeOutputDirectory(output_dir);
}

// Changes the output directory
// Also used by constructor to set default output directory
void FileGenerator::ChangeOutputDirectory(const std::string& output_dir) {
  // First, check if it is a valid directory
  boost::filesystem::path dir(output_dir);
  if (boost::filesystem::exists(dir) && boost::filesystem::is_directory(dir)) {
    outdir_ = dir.string();
  } else {
    throw flame::exceptions::invalid_argument("Not a valid directory");
  }
}

// outfile can contain a subdirectory path. However, do not create missing
// subdirs so the caller has to make sure the directory exists
void FileGenerator::Output(const std::string& outfile,
                           const xparser::codegen::CodeGenerator& g) const {
  // compose full path to output file
  boost::filesystem::path outdir(outdir_);
  boost::filesystem::path outpath = outdir / outfile;

  // open file for writing
  std::ofstream f(outpath.string().c_str());
  if (!f.is_open()) {
    throw flame::exceptions::invalid_argument("Cannot open file for writing");
  }

  // instantiate printer and dump code into it
  xparser::Printer printer(&f);
  g.Generate(&printer);
  f.close();
}

}  // namespace xparser
