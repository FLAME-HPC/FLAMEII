/*!
 * \file flame2/io/plugins/io_cli_pop.cpp
 * \author Simon Coakley
 * \date 2013
 * \copyright Copyright (c) 2013 STFC Rutherford Appleton Laboratory
 * \copyright Copyright (c) 2013 University of Sheffield
 * \copyright GNU Lesser General Public License
 * \brief IOCLIPop: writing of population to CLI
 */
#include <cstdio>
#include <string>
#include <stdexcept>
#include "io_cli_pop.hpp"
#include "flame2/config.hpp"

namespace flame { namespace io {

std::string IOCLIPop::getName() {
  return "cli";
}

void IOCLIPop::readPop(std::string /*path*/,
    void (*)(std::string const&, std::string const&, int),
    void (*)(std::string const&, std::string const&, double)) {
  // throw error
  throw std::runtime_error(
      "IO plugin cli cannot be used to read a pop file");
  // could read in data from command line via cin?
}

void IOCLIPop::initialiseData() {}

void IOCLIPop::writePop(std::string const& agent_name,
    std::string const& var_name, size_t size, void * ptr) {
  size_t ii;
  std::string var_type = getVariableType(agent_name, var_name);

  for (ii = 0; ii < size; ++ii) {
    printf("%s\t%s\t", agent_name.c_str(), var_name.c_str());
    if (var_type == "int")
      printf("%d\n", *(static_cast<int*>(ptr)+ii));
    if (var_type == "double")
      printf("%f\n", *(static_cast<double*>(ptr)+ii));
  }
}

void IOCLIPop::finaliseData() {}

}}  // namespace flame::io
