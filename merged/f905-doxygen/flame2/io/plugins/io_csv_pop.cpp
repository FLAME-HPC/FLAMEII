/*!
 * \file flame2/io/plugins/io_csv_pop.cpp
 * \author Simon Coakley
 * \date 2013
 * \copyright Copyright (c) 2013 STFC Rutherford Appleton Laboratory
 * \copyright Copyright (c) 2013 University of Sheffield
 * \copyright GNU Lesser General Public License
 * \brief IOCSVPop: writing of population to CSV file
 */
#include <stdlib.h>
#include <stdio.h>
#include <stdexcept>
#include <cstdlib>
#include <string>
#include <utility>
#include <map>
#include <iostream>
#include <fstream>
#include <sstream>
#include "io_csv_pop.hpp"
#include "flame2/config.hpp"

namespace flame { namespace io {

std::string IOCSVPop::getName() {
  return "csv";
}

void IOCSVPop::readPop(std::string path,
    void (*addInt)(std::string const&, std::string const&, int),
    void (*addDouble)(std::string const&, std::string const&, double)) {
  AgentMemory::iterator ait;
  VarVec::iterator vit;

  // open file as input stream
  std::ifstream file(path.c_str());
  std::string value;
  std::string line;
  std::stringstream iss;
  // get each line from the file at a time
  while (std::getline(file, line)) {
    // pass the line to a string stream
    iss << line;
    // read the string stream until next comma
    std::getline(iss, value, ',');
    for (ait = agentMemory_.begin(); ait != agentMemory_.end(); ++ait) {
      // if the first value equals an agent type
      if (value == ait->first) {
        // for each variable
        for (vit = ait->second.begin(); vit != ait->second.end(); ++vit) {
          // get next value
          std::getline(iss, value, ',');
          // if the value is an int
          if (vit->first == "int")
            addInt(ait->first, vit->second, std::atoi(value.c_str()));
          // if the value is a double
          if (vit->first == "double")
            addDouble(ait->first, vit->second, std::atof(value.c_str()));
        }
      }
    }
    // clear the string stream ready for the next line
    iss.clear();
  }
  // close the file
  file.close();
}

void IOCSVPop::initialiseData() {}

void IOCSVPop::writePop(std::string const& agent_name,
    std::string const& var_name, size_t size, void * ptr) {
  // get agent
  AgentMemoryArrays::iterator ait;
  ait = agentMemoryArrays_.find(agent_name);
  if (ait == agentMemoryArrays_.end())
    ait = agentMemoryArrays_.insert(
        std::make_pair(agent_name, VarPtrArrayMap())).first;
  // get variable
  VarPtrArrayMap * vpam = &(*ait).second;
  VarPtrArrayMap::iterator vit;
  vit = vpam->find(var_name);
  if (vit == vpam->end())
    vit = vpam->insert(std::make_pair(var_name, PtrArray())).first;
  // set pointer to array and size
  (*vit).second.first = ptr;
  (*vit).second.second = size;
}

void IOCSVPop::writeAgents(FILE * fp) {
  AgentMemory::iterator ait;
  VarVec::iterator vit;
  // for each agent type
  for (ait = agentMemory_.begin(); ait != agentMemory_.end(); ++ait) {
    std::string agent_name = ait->first;
    VarPtrArrayMap * vpam = &(*agentMemoryArrays_.find(agent_name)).second;
    size_t ii, size = 0;
    // get a size from first variable if exists
    if (vpam->size() > 0) size = vpam->begin()->second.second;
    // for each agent instance
    for (ii = 0; ii < size; ++ii) {
      // write agent name
      fprintf(fp, "%s", agent_name.c_str());
      // for each variable
      for (vit = ait->second.begin(); vit != ait->second.end(); ++vit) {
        std::string var_name = vit->second;
        VarPtrArrayMap::iterator vpamit = vpam->find(var_name);
        // find var type
        VarVec::iterator vvit;
        std::string var_type = getVariableType(agent_name, var_name);
        if (var_type == "int") fprintf(fp, ",%d",
            *(static_cast<int*>(vpamit->second.first)+ii));
        if (var_type == "double") fprintf(fp, ",%f",
            *(static_cast<double*>(vpamit->second.first)+ii));
      }
      fprintf(fp, "\n");
    }
  }
}

void IOCSVPop::finaliseData() {
  char str[32];
  snprintf(str, sizeof(str), "%d", static_cast<int>(iteration_));
  std::string file_name = path_;
  file_name.append(str);
  file_name.append(".csv");
  // open file to write to
  FILE *fp;
  fp = fopen(file_name.c_str(), "w");
  if (fp == NULL) throw std::runtime_error("Cannot open file to write to");
  // write agents
  writeAgents(fp);
  // close file
  fclose(fp);
  // clear var array data ready for next iteration
  agentMemoryArrays_.clear();
}

}}  // namespace flame::io
