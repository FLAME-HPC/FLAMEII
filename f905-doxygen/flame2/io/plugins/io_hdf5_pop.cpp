/*!
 * \file flame2/io/plugins/io_hdf5_pop.cpp
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
#include "io_hdf5_pop.hpp"
#include "flame2/config.hpp"

//! \brief Macro to do HDF5 error checking
#define H5_ERR_CHECK(func)                                                 \
do {                                                                       \
  herr_t h5status = func;                                                  \
  if (h5status < 0) {                                                      \
    fprintf(stderr, "H5IO: ERROR:%s:%i: %s\n", __FILE__, __LINE__, #func); \
    throw std::runtime_error("hdf5 error");                                \
  }                                                                        \
}while(0)

namespace flame { namespace io {

std::string IOHDF5Pop::getFileName() {
  std::string file_name = path_;
  std::ostringstream convert;
  convert << iteration_;
  file_name.append(convert.str());
  file_name.append(".hdf5");

  return file_name;
}

std::string IOHDF5Pop::getName() {
  return "hdf5";
}

//! Validate file
void IOHDF5Pop::validate(hid_t h5file) {
  AgentMemory::iterator ait;
  VarVec::iterator vit;

  bool valid = true;
  // check agents data set exists
  if (H5Lexists(h5file, "/agents", H5P_DEFAULT) == 1) {
    // for each agent
    for (ait = agentMemory_.begin(); ait != agentMemory_.end(); ++ait) {
      // set data set name
      std::string dataset = "/agents/";
      dataset.append(ait->first);
      // check agent data set exists
      if (H5Lexists(h5file, dataset.c_str(), H5P_DEFAULT) == 1) {
        for (vit = ait->second.begin(); vit != ait->second.end(); ++vit) {
          // set data set name
          std::string data = dataset.append("/");
          data.append(vit->second);
          // check agent variable data  exists
          if (H5Lexists(h5file, data.c_str(), H5P_DEFAULT) != 1) {
            printf("ERROR: %s data not found.\n", data.c_str());
            valid = false;
          }
        }
      } else {
        printf("ERROR: %s dataset not found.\n", dataset.c_str());
        valid = false;
      }
    }
  } else {
    printf("ERROR: /agents dataset not found.\n");
    valid = false;
  }

  // if file is not valid throw error
  if (!valid) throw std::runtime_error("HDF5 file is not valid");
}

void IOHDF5Pop::readIntDataset(hid_t h5dataset, size_t size,
    std::string const& agent_name, std::string const& var_name,
    void (*addInt)(std::string const&, std::string const&, int)) {
  // buffer for the read in value
  int value;
  // loop number
  size_t ii;
  // dimension of mem space needed for reading in data
  const hsize_t dim[1] = {1};
  // the offset of the element being read
  hsize_t start_src[1];
  // the space selection of the element to read
  hid_t space_id;
  // the creation of the mem space for the read data
  hid_t mem_space_id = H5Screate_simple(1, dim, NULL);
  // loop though the size of the array
  for (ii = 0; ii < size; ++ii) {
    // get the file data space
    space_id = H5Dget_space(h5dataset);
    // set the offset of the element
    start_src[0] = ii;
    // set the data space to be only the element we want to read
    H5Sselect_elements(space_id, H5S_SELECT_SET, 1, start_src);
    // read the element into value
    H5_ERR_CHECK(H5Dread(h5dataset, H5T_NATIVE_INT,
        mem_space_id, space_id, H5P_DEFAULT, &value));
    // add the value
    addInt(agent_name, var_name, value);
    // close the data space
    H5Sclose(space_id);
  }
  // close the mem space
  H5Sclose(mem_space_id);
}

void IOHDF5Pop::readDoubleDataset(hid_t h5dataset, size_t size,
    std::string const& agent_name, std::string const& var_name,
    void (*addDouble)(std::string const&, std::string const&, double)) {
  // buffer for the read in value
  double value;
  // loop number
  size_t ii;
  // dimension of mem space needed for reading in data
  const hsize_t dim[1] = {1};
  // the offset of the element being read
  hsize_t start_src[1];
  // the space selection of the element to read
  hid_t space_id;
  // the creation of the mem space for the read data
  hid_t mem_space_id = H5Screate_simple(1, dim, NULL);
  // loop though the size of the array
  for (ii = 0; ii < size; ++ii) {
    // get the file data space
    space_id = H5Dget_space(h5dataset);
    // set the offset of the element
    start_src[0] = ii;
    // set the data space to be only the element we want to read
    H5Sselect_elements(space_id, H5S_SELECT_SET, 1, start_src);
    // read the element into value
    H5_ERR_CHECK(H5Dread(h5dataset, H5T_NATIVE_DOUBLE,
        mem_space_id, space_id, H5P_DEFAULT, &value));
    // add the value
    addDouble(agent_name, var_name, value);
    // close the data space
    H5Sclose(space_id);
  }
  // close the mem space
  H5Sclose(mem_space_id);
}

void IOHDF5Pop::readPop(std::string path,
    void (*addInt)(std::string const&, std::string const&, int),
    void (*addDouble)(std::string const&, std::string const&, double)) {
  AgentMemory::iterator ait;
  VarVec::iterator vit;

  // check file is hdf5
  if (!H5Fis_hdf5(path.c_str())) throw std::runtime_error("File not HDF5 file");

  // open hdf5 file
  hid_t h5file = H5Fopen(path.c_str(), H5F_ACC_RDONLY, H5P_DEFAULT);
  // validate the hdf5 file
  validate(h5file);
  // read data from file
  for (ait = agentMemory_.begin(); ait != agentMemory_.end(); ++ait)
    for (vit = ait->second.begin(); vit != ait->second.end(); ++vit) {
      // set agent variable data set name
      std::string data = "agents/";
      data.append(ait->first);
      data.append("/");
      data.append(vit->second);

      // open data set
      hid_t h5dataset = H5Dopen1(h5file, data.c_str());

      // find size of array
      hsize_t dims_out[2];
      hid_t dataspace = H5Dget_space(h5dataset);
      // int status_n =
          H5Sget_simple_extent_dims(dataspace, dims_out, NULL);
      size_t size = (size_t)(dims_out[0]);

      // read data
      if (vit->first == "int") readIntDataset(
          h5dataset, size, ait->first, vit->second, addInt);
      if (vit->first == "double") readDoubleDataset(
          h5dataset, size, ait->first, vit->second, addDouble);

      // close data set
      H5_ERR_CHECK(H5Dclose(h5dataset));
    }
  // close file
  H5_ERR_CHECK(H5Fclose(h5file));
}

void IOHDF5Pop::initialiseData() {
  // open the hdf5 file
  hid_t h5file = H5Fcreate(
      getFileName().c_str(), H5F_ACC_TRUNC, H5P_DEFAULT, H5P_DEFAULT);
  // create a property list
  hid_t h5lcprop = H5Pcreate(H5P_LINK_CREATE);
  // set property to create missing groups
  H5_ERR_CHECK(H5Pset_create_intermediate_group(h5lcprop, 1));

  // create agents group
  hid_t h5groupa = H5Gcreate1(h5file, "/agents", 0);
  // close agents group
  H5_ERR_CHECK(H5Gclose(h5groupa));

  // for each agent
  AgentMemory::iterator ait;
  for (ait = agentMemory_.begin(); ait != agentMemory_.end(); ++ait) {
    // set the data set name
    std::string name("/agents/");
    name.append(ait->first);
    // create the agent data group
    hid_t h5group = H5Gcreate1(h5file, name.c_str(), 0);
    // close the data group
    H5_ERR_CHECK(H5Gclose(h5group));
  }
  // close the file
  H5_ERR_CHECK(H5Fclose(h5file));
}

void IOHDF5Pop::writePop(std::string const& agent_name,
    std::string const& var_name, size_t size, void * ptr) {
  // open the hdf5 file
  hid_t file = H5Fopen(getFileName().c_str(), H5F_ACC_RDWR, H5P_DEFAULT);
  // create a data space with dimension one and length size
  hsize_t dims[] = {size};
  hid_t h5dataspace = H5Screate_simple(1, dims, NULL);

  // set the dataset name
  std::string name("/agents/");
  name.append(agent_name);
  name.append("/");
  name.append(var_name);

  // find the variable type
  std::string var_type = getVariableType(agent_name, var_name);

  hid_t mem_type_id;  // the data type
  // set the data type
  if (var_type == "int") mem_type_id = H5T_NATIVE_INT;
  if (var_type == "double") mem_type_id = H5T_NATIVE_DOUBLE;
  // create the data set
  hid_t h5dataset = H5Dcreate1(file, name.c_str(), mem_type_id, h5dataspace,
      H5P_DEFAULT);
  // write the data
  H5_ERR_CHECK(H5Dwrite(
      h5dataset, mem_type_id, H5S_ALL, H5S_ALL, H5P_DEFAULT, ptr));
  // close the data set
  H5_ERR_CHECK(H5Dclose(h5dataset));
  // close the file
  H5_ERR_CHECK(H5Fclose(file));
}

void IOHDF5Pop::finaliseData() {}

}}  // namespace flame::io

#endif
