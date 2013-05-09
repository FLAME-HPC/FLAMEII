/*!
 * \file flame2/io/plugins/io_hdf5_pop.hpp
 * \author Simon Coakley
 * \date 2013
 * \copyright Copyright (c) 2013 STFC Rutherford Appleton Laboratory
 * \copyright Copyright (c) 2013 University of Sheffield
 * \copyright GNU Lesser General Public License
 * \brief IOHDF5Pop: writing of population to and from HDF5
 */
#ifdef HAVE_HDF5
#include <hdf5.h>     // HDF5 header
#include <hdf5_hl.h>  // HDF5 High Level include file
#include <string>
#include <sstream>
#include <stdexcept>
#include "../io_interface.hpp"

namespace flame { namespace io {

class IOHDF5Pop : public IO {
    std::string getFileName();
    std::string getName();
    //! Validate file
    void validate(hid_t h5file);
    void readIntDataset(hid_t h5dataset, size_t size,
        std::string const& agent_name, std::string const& var_name,
        void (*addInt)(std::string const&, std::string const&, int));
    void readDoubleDataset(hid_t h5dataset, size_t size,
        std::string const& agent_name, std::string const& var_name,
        void (*addDouble)(std::string const&, std::string const&, double));
    //! Reading method, called by io manager
    void readPop(std::string path,
        void (*addInt)(std::string const&, std::string const&, int),
        void (*addDouble)(std::string const&, std::string const&, double));
    //! Initialise writing out of data for an iteration
    void initialiseData();
    //! Write out an agent variable for all agents
    void writePop(std::string const& agent_name,
        std::string const& var_name, size_t size, void * ptr);
    //! Finalise writing out of data for an iteration
    void finaliseData();

  private:
    hid_t h5_Circle_datatype;
};

}}  // namespace flame::io
#endif
