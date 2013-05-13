/*!
 * \file flame2/io/plugins/io_csv_pop.hpp
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
#include "../io_interface.hpp"

namespace flame { namespace io {

typedef std::pair<void*, size_t> PtrArray;
typedef std::map<std::string, PtrArray> VarPtrArrayMap;
typedef std::map<std::string, VarPtrArrayMap> AgentMemoryArrays;

/*!
 * \brief IO plugin for reading and writing CSV files
 */
class IOCSVPop : public IO {
    std::string getName();
    //! Reading method, called by io manager
    void readPop(std::string path,
        void (*addInt)(std::string const&, std::string const&, int),
        void (*addDouble)(std::string const&, std::string const&, double));
    //! Initialise writing out of data for an iteration
    void initialiseData();
    //! Write out an agent variable for all agents
    void writePop(std::string const& agent_name,
        std::string const& var_name, size_t size, void * ptr);
    //! Write out agents to file
    void writeAgents(FILE * fp);
    //! Finalise writing out of data for an iteration
    void finaliseData();

  private:
    AgentMemoryArrays agentMemoryArrays_;
};

}}  // namespace flame::io
