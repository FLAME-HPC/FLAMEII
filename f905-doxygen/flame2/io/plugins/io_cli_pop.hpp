/*!
 * \file flame2/io/plugins/io_cli_pop.hpp
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
#include "../io_interface.hpp"

namespace flame { namespace io {

/*!
 * \brief IO class for outputting agent memories to the command line
 */
class IOCLIPop : public IO {
    std::string getName();
    /*!
     * \brief Reading method, called by io manager
     *
     * \warning Throws exception as not implemented
    */
    void readPop(std::string /*path*/,
        void (*)(std::string const&, std::string const&, int),
        void (*)(std::string const&, std::string const&, double));
    //! Initialise writing out of data for an iteration
    void initialiseData();
    //! Write out an agent variable for all agents
    void writePop(std::string const& agent_name,
        std::string const& var_name, size_t size, void * ptr);
    //! Finalise writing out of data for an iteration
    void finaliseData();
};

}}  // namespace flame::io
