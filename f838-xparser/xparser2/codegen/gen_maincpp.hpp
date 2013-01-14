/*!
 * \file xparser2/gen_maincpp.hpp
 * \author Shawn Chin
 * \date January 2013
 * \copyright Copyright (c) 2013 STFC Rutherford Appleton Laboratory
 * \copyright Copyright (c) 2013 University of Sheffield
 * \copyright GNU Lesser General Public License
 * \brief generator for main.cpp file to drive flame simulation
 */
#ifndef XPARSER__CODEGEN__GEN_MAINCPP_HPP_
#define XPARSER__CODEGEN__GEN_MAINCPP_HPP_
#include "gen_file.hpp"
namespace xparser { namespace codegen {

//! Generates main.ccp file
class GenMainCpp : public GenFile {
  public:
    GenMainCpp();
    void Generate(Printer* printer) const;
};
}}  // namespace xparser::codegen
#endif  // XPARSER__CODEGEN__GEN_MAINCPP_HPP_

