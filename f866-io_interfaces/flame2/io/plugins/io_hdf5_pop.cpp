/*!
 * \file flame2/io/plugins/io_hdf5_pop.cpp
 * \author Simon Coakley
 * \date 2013
 * \copyright Copyright (c) 2013 STFC Rutherford Appleton Laboratory
 * \copyright Copyright (c) 2013 University of Sheffield
 * \copyright GNU Lesser General Public License
 * \brief IOHDF5Pop: writing of population to and from HDF5
 */
#include <string>
#include <stdexcept>
#include "hdf5.h"  // HDF5 header
#include "hdf5_hl.h"  // HDF5 High Level include file
#include "../io_interface.hpp"

// #include "flame2/config.hpp"  // Needed?

namespace flame { namespace io {

class IOHDF5Pop : public IO {

#define H5_ERR_CHECK(func)                                                 \
do {                                                                       \
  herr_t h5status = func;                                                  \
  if(h5status < 0) {                                                       \
    fprintf(stderr, "H5IO: ERROR:%s:%i: %s\n", __FILE__, __LINE__, #func); \
    exit(EXIT_FAILURE);                                                    \
  }                                                                        \
}while(0)

    std::string getFileName() {
      char str[32];
      snprintf(str, sizeof(str), "%lu", iteration_);
      std::string file_name = path_;
      file_name.append(str);
      file_name.append(".hdf5");
      return file_name;
    }

    std::string getName() {
      return "hdf5";
    }

    /*static herr_t
    test_select_hyper_iter1(void *_elem, hid_t UNUSED type_id, unsigned UNUSED ndim, const hsize_t UNUSED *point, void *_operator_data)
    {
        uint8_t *tbuf=(uint8_t *)_elem,     // temporary buffer pointer
                **tbuf2=(uint8_t **)_operator_data; // temporary buffer handle

        if(*tbuf!=**tbuf2)
            return(-1);
        else {
            (*tbuf2)++;
            return(0);
        }
    }*/

    static herr_t test_select_hyper_iter1(void *elem, hid_t type_id, unsigned ndim,
                             const hsize_t *point, void *operator_data) {
      printf("here\n");
      return(0);
    }

    //! Reading method, called by io manager
    void readPop(std::string path,
        void (*addInt)(std::string const&, std::string const&, int),
        void (*addDouble)(std::string const&, std::string const&, double)) {
      AgentMemory::iterator ait;
      VarVec::iterator vit;
      hid_t h5file = NULL;

      if(H5Fis_hdf5(path.c_str()))
      {
        h5file = H5Fopen(path.c_str(), H5F_ACC_RDONLY, H5P_DEFAULT);

        // validate hdf5 file
        bool valid = true;
        if(H5Lexists(h5file, "/agents", H5P_DEFAULT) == 1) {
          //for each agent
          for (ait = agentMemory_.begin(); ait != agentMemory_.end(); ++ait) {
            std::string dataset = "/agents/";
            dataset.append(ait->first);
            if(H5Lexists(h5file, dataset.c_str(), H5P_DEFAULT) == 1) {
              for (vit = ait->second.begin(); vit != ait->second.end(); ++vit) {
                std::string data = dataset.append("/");
                data.append(vit->second);
                if(H5Lexists(h5file, data.c_str(), H5P_DEFAULT) != 1) {
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

        // if valid file
        if (valid) {
          for (ait = agentMemory_.begin(); ait != agentMemory_.end(); ++ait) {
            for (vit = ait->second.begin(); vit != ait->second.end(); ++vit) {
              std::string data = "agents/";
              data.append(ait->first);
              data.append("/");
              data.append(vit->second);

              hid_t h5dataset = H5Dopen(h5file, data.c_str(), H5P_DEFAULT);

              hsize_t dims_out[2];           /* dataset dimensions */
              hid_t dataspace = H5Dget_space(h5dataset);    /* dataspace handle */
              int status_n  = H5Sget_simple_extent_dims(dataspace, dims_out, NULL);
              size_t size = (size_t)(dims_out[0]);

              if (vit->first == "int") {
                // allocate space
                int * buf = (int *) malloc(size*sizeof(int));
                H5_ERR_CHECK(H5Dread(h5dataset, H5T_NATIVE_INT,
                    H5S_ALL,  // mem_space_id
                    H5S_ALL,  // file_space_id
                    H5P_DEFAULT, buf));
                for (size_t ii = 0; ii < size; ++ii)
                  addInt(ait->first, vit->second, *(buf+ii));
                // free space
                delete buf;
              }
              if (vit->first == "double") {
                // allocate space
                double * buf = (double *) malloc(size*sizeof(double));
                H5_ERR_CHECK(H5Dread(h5dataset, H5T_NATIVE_DOUBLE,
                    H5S_ALL,  // mem_space_id
                    H5S_ALL,  // file_space_id
                    H5P_DEFAULT, buf));
                for (size_t ii = 0; ii < size; ++ii)
                  addDouble(ait->first, vit->second, *(buf+ii));
                // free space
                delete buf;
              }

              H5_ERR_CHECK(H5Dclose(h5dataset));
            }
          }
        } else {
          throw std::runtime_error("HDF5 file is not valid");
        }

      }
      else throw std::runtime_error("File not in HDF5 format");

      H5_ERR_CHECK(H5Fclose(h5file));
    }

    //! Initialise writing out of data for an iteration
    void initialiseData() {
      hid_t h5file;       /* H5 File Identifier */
        hid_t h5group;      /* H5 Group Identifier */
        hid_t h5lcprop;     /* H5 Link Creation Property */
        std::string file_name = getFileName();

        h5file = H5Fcreate(file_name.c_str(), H5F_ACC_TRUNC, H5P_DEFAULT, H5P_DEFAULT);

        h5lcprop = H5Pcreate(H5P_LINK_CREATE);

        //for each agent
        AgentMemory::iterator ait;
        for (ait = agentMemory_.begin(); ait != agentMemory_.end(); ++ait) {
          std::string name("/agents/");
          name.append(ait->first);
          H5_ERR_CHECK(H5Pset_create_intermediate_group(h5lcprop, 1));
          h5group = H5Gcreate(h5file, name.c_str(), h5lcprop, H5P_DEFAULT, H5P_DEFAULT);
          H5_ERR_CHECK(H5Gclose(h5group));
        }

        H5_ERR_CHECK(H5Fclose(h5file));
    }
    //! Write out an agent variable for all agents
    void writePop(std::string const& agent_name,
        std::string const& var_name, size_t size, void * ptr) {
      hid_t file;
      std::string file_name = getFileName();

      file = H5Fopen(file_name.c_str(), H5F_ACC_RDWR, H5P_DEFAULT);
      // open an existing dataset
      hsize_t   h5Circle_dims[] = {size};
      hid_t h5dataspace = H5Screate_simple(1, h5Circle_dims, NULL);

      std::string name("/agents/");
      name.append(agent_name);
      name.append("/");
      name.append(var_name);

      // find var type
      std::string var_type = getVariableType(agent_name, var_name);

      hid_t h5dataset;
      hid_t mem_type_id;
      if (var_type == "int") mem_type_id = H5T_NATIVE_INT;
      if (var_type == "double") mem_type_id = H5T_NATIVE_DOUBLE;
      h5dataset = H5Dcreate(file, name.c_str(), mem_type_id, h5dataspace, H5P_DEFAULT, H5P_DEFAULT, H5P_DEFAULT);
      H5_ERR_CHECK(H5Dwrite(h5dataset, mem_type_id, H5S_ALL, H5S_ALL, H5P_DEFAULT, ptr));

      H5Dclose(h5dataset);

      H5_ERR_CHECK(H5Fclose(file));
    }
    //! Finalise writing out of data for an iteration
    void finaliseData() {}

  private:
    hid_t h5_Circle_datatype;
};

// set 'C' linkage for function names
extern "C" {
  // function to return an instance of a new IO plugin object
  IO* getIOPlugin() {
    return new IOHDF5Pop();
  }
}
}}  // namespace flame::io
