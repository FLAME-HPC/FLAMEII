/*!
 * \file flame2/io/plugins/io_cli_pop.cpp
 * \author Simon Coakley
 * \date 2013
 * \copyright Copyright (c) 2013 STFC Rutherford Appleton Laboratory
 * \copyright Copyright (c) 2013 University of Sheffield
 * \copyright GNU Lesser General Public License
 * \brief IOCLIPop: writing of population to CLI
 */
#include <string>
#include "../io_interface.hpp"

// #include "flame2/config.hpp"  // Needed?

namespace flame { namespace io {

class IOCLIPop : public IO {
    std::string getName() {
      return "cli";
    }
    //! Reading method, called by io manager
    void readPop(std::string path,
        void (*addInt)(std::string const&, std::string const&, int),
        void (*addDouble)(std::string const&, std::string const&, double)) {}
    //! Initialise writing out of data for an iteration
    void initialiseData() {}
    //! Write out an agent variable for all agents
    void writePop(std::string const& agent_name,
        std::string const& var_name, size_t size, void * ptr) {
      AgentMemory::iterator ait;
      size_t ii;

      ait = agentMemory_.find(agent_name);
      if (ait != agentMemory_.end()) {
        VarVec::iterator vvit;
        // for each agent type
        for (vvit = ait->second.begin(); vvit != ait->second.end(); ++vvit) {
          if ((*vvit).second == var_name) {
            for (ii = 0; ii < size; ++ii) {
              printf("%s\t%s\t", agent_name.c_str(), var_name.c_str());
              if ((*vvit).first == "int")
                printf("%d\n", *(static_cast<int*>(ptr)+ii));
              if ((*vvit).first == "double")
                printf("%f\n", *(static_cast<double*>(ptr)+ii));
            }
          }
        }
      }
    }
    //! Finalise writing out of data for an iteration
    void finaliseData() {}
};

// set 'C' linkage for function names
extern "C" {
  // function to return an instance of a new IO plugin object
  IO* getIOPlugin() {
    return new IOCLIPop();
  }
}
}}  // namespace flame::io
