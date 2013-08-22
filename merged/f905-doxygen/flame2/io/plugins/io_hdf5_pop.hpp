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

/*!
 * \brief IO Plugin to read and write to HDF5 files
 */
class IOHDF5Pop : public IOInterface {
  public:
    /*!
     * \brief Constructor
     */
    IOHDF5Pop() {}
    /*!
     * \brief Return plugin name
     * \return Plugin name
     */
    std::string getName();
    /*!
     * \brief Validate file
     * \param[in] h5file File to validate
     */
    void validate(hid_t h5file);
    /*!
     * \brief Read HDF5 population file
     * \param[in] path File to read
     * \param[out] addInt Function to add integers
     * \param[out] addDouble Function to add doubles
     */
    void readPop(std::string path,
        void (*addInt)(std::string const&, std::string const&, int),
        void (*addDouble)(std::string const&, std::string const&, double));
    //! \brief Initialise writing out of data for an iteration
    void initialiseData();
    /*!
     * \brief Write agent variable array to HDF5 file
     * \param[in] agent_name The agent name
     * \param[in] var_name The variable name
     * \param[in] size The size of the variable array
     * \param[in] ptr Pointer to the variable array
     */
    void writePop(std::string const& agent_name,
        std::string const& var_name, size_t size, void * ptr);
    //! \brief Finalise writing out of data for an iteration
    void finaliseData();

  private:
    /*!
     * \brief Return file name
     * \return File name
     */
    std::string getFileName();
    /*!
     * \brief Read integer data set
     * \param[in] h5dataset Data set to read
     * \param[in] size Vector size
     * \param[in] agent_name Agent name
     * \param[in] var_name Variable name
     * \param[out] addInt Function to add integers
     */
    void readIntDataset(hid_t h5dataset, size_t size,
        std::string const& agent_name, std::string const& var_name,
        void (*addInt)(std::string const&, std::string const&, int));
    /*!
     * \brief Read double data set
     * \param[in] h5dataset Data set to read
     * \param[in] size Vector size
     * \param[in] agent_name Agent name
     * \param[in] var_name Variable name
     * \param[out] addDouble Function to add doubles
     */
    void readDoubleDataset(hid_t h5dataset, size_t size,
        std::string const& agent_name, std::string const& var_name,
        void (*addDouble)(std::string const&, std::string const&, double));
};

}}  // namespace flame::io
#endif
