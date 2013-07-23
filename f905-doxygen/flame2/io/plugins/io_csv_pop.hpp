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
class IOCSVPop : public IOInterface {
  public:
    //! \brief Constructor, initialise data members
    IOCSVPop() : agentMemoryArrays_() {}
    /*!
     * \brief Get the IO plugin name
     * \return Plugin name
     */
    std::string getName();
    /*!
     * \brief Reading method, called by io manager
     * \param[in] path File to read
     * \param[out] addInt() Function to add integers
     * \param[out] addDouble() Function to add doubles
     */
    void readPop(std::string path,
        void (*addInt)(std::string const&, std::string const&, int),
        void (*addDouble)(std::string const&, std::string const&, double));
    /*!
     * \brief Initialise writing out of data for an iteration
     */
    void initialiseData();
    /*!
     * \brief Write out an agent variable for all agents
     * \param[in] agent_name The agent name
     * \param[in] var_name The variable name
     * \param[in] size The size of the variable array
     * \param[in] ptr Pointer to the variable array
     */
    void writePop(std::string const& agent_name,
        std::string const& var_name, size_t size, void * ptr);
    /*!
     * \brief Write out agents to file
     * \param[in] fp File pointer
     */
    void writeAgents(FILE * fp);
    /*!
     * \brief Finalise writing out of data for an iteration
     */
    void finaliseData();

  private:
    //! \brief Agent memory information
    AgentMemoryArrays agentMemoryArrays_;
};

}}  // namespace flame::io
