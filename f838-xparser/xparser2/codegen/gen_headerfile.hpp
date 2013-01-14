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
#include "gen_file.hpp"
namespace xparser { namespace codegen {

/*! \brief Generates a header file
 *
 * Similar to GenFile, but wraps content with an randomly generated
 * include guard.
 */
class GenHeaderFile : public GenFile {
  public:
    //! Writes header file to printer instance
    void Generate(Printer* printer) const;
};
}}  // namespace xparser::codegen
#endif  // XPARSER__CODEGEN__GEN_HEADERFILE_HPP_
