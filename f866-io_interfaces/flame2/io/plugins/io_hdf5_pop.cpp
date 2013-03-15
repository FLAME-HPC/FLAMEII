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
    std::string getName() {
      return "hdf5";
    }
    //! Reading method, called by io manager
    void readPop(std::string path,
        void (*addInt)(std::string const&, std::string const&, int),
        void (*addDouble)(std::string const&, std::string const&, double)) {

    }

#define H5_ERR_CHECK(func)                                                             \
do {                                                                                   \
  herr_t h5status = func;                                                               \
  if(h5status < 0) {                                                             \
    fprintf(stderr, "H5IO: ERROR:%s:%i: %s\n", __FILE__, __LINE__, #func); \
    exit(EXIT_FAILURE);                                                    \
  }                                                                              \
}while(0)

    typedef struct _h5_Circle_dataset
    {
      int id;
      double x;
      double y;
      double fx;
      double fy;
      double radius;
    } h5Circle_t;

    hid_t h5make_Circle_datatype()
    {
      printf("Making Circle Datatype...\n");
      h5_Circle_datatype = H5Tcreate (H5T_COMPOUND, sizeof(h5Circle_t));
      printf("Inserting (int)id...\n");
      H5_ERR_CHECK(H5Tinsert(h5_Circle_datatype, "id", HOFFSET(h5Circle_t, id), H5T_NATIVE_INT));
      printf("Inserting (double)x...\n");
      H5_ERR_CHECK(H5Tinsert(h5_Circle_datatype, "x", HOFFSET(h5Circle_t, x), H5T_NATIVE_DOUBLE));
      printf("Inserting (double)y...\n");
      H5_ERR_CHECK(H5Tinsert(h5_Circle_datatype, "y", HOFFSET(h5Circle_t, y), H5T_NATIVE_DOUBLE));
      printf("Inserting (double)fx...\n");
      H5_ERR_CHECK(H5Tinsert(h5_Circle_datatype, "fx", HOFFSET(h5Circle_t, fx), H5T_NATIVE_DOUBLE));
      printf("Inserting (double)fy...\n");
      H5_ERR_CHECK(H5Tinsert(h5_Circle_datatype, "fy", HOFFSET(h5Circle_t, fy), H5T_NATIVE_DOUBLE));
      printf("Inserting (double)radius...\n");
      H5_ERR_CHECK(H5Tinsert(h5_Circle_datatype, "radius", HOFFSET(h5Circle_t, radius), H5T_NATIVE_DOUBLE));

      printf("Circle Datatype Ready.\n");
      return h5_Circle_datatype;
    }

    void h5write_Circle(hid_t h5file)
    {
      hid_t h5dataset;            /* Dataset Identifier */
      hid_t h5dataspace;            /* Dataspace Identifier */

      unsigned int Circle_count = 5; //Circle_end_state->count + 0;

      hsize_t   h5Circle_dims[] = {Circle_count};               /* Circle Dimensions */
      h5Circle_t  h5Circle[Circle_count];                        /* Circle Compound Data*/
      printf("Preparing to Write %i Circle.\n",Circle_count);

      printf("Creating a Dataspace...\n");
      h5dataspace = H5Screate_simple(1, h5Circle_dims, NULL);

      printf("Creating the Circle Dataset Headers...\n");
      h5dataset = H5Dcreate(h5file, "/agents/Circle", h5_Circle_datatype, h5dataspace, H5P_DEFAULT, H5P_DEFAULT, H5P_DEFAULT);

      printf("Populating internal HDF5 buffers...\n");
      unsigned int i = 0;
      for (i = 0; i < Circle_count; ++i) {
        h5Circle[i].id = i;
        h5Circle[i].x = 0.1;
        h5Circle[i].y = 0.2;
        h5Circle[i].fx = 0.3;
        h5Circle[i].fy = 0.4;
        h5Circle[i].radius = 0.5;
      }

      printf("Internal buffers populated.\n");

      printf("Writing Circle Dataset...\n");
      H5_ERR_CHECK(H5Dwrite(h5dataset, h5_Circle_datatype, H5S_ALL, H5S_ALL, H5P_DEFAULT, h5Circle));
      printf("Creating an 'count' Attribute...\n");
      H5_ERR_CHECK(H5LTset_attribute_uint(h5file, "/agents/Circle", "count", &Circle_count, 1));
      printf("Releasing Resources...\n");
      H5_ERR_CHECK(H5Sclose(h5dataspace));
      H5_ERR_CHECK(H5Dclose(h5dataset));
    }

    //! Initialise writing out of data for an iteration
    void initialiseData() {
      hid_t h5file;       /* H5 File Identifier */
        hid_t h5group;      /* H5 Group Identifier */
        hid_t h5lcprop;     /* H5 Link Creation Property */

        char str[32];
        snprintf(str, sizeof(str), "%lu", iteration_);
        std::string file_name = path_;
        file_name.append(str);
        file_name.append(".hdf5");

        printf("Writing to File: %s\n", file_name.c_str());
        h5file = H5Fcreate(file_name.c_str(), H5F_ACC_TRUNC, H5P_DEFAULT, H5P_DEFAULT);

        printf("Creating /states/xagents Group...\n");
        h5lcprop = H5Pcreate(H5P_LINK_CREATE);
        printf("Creating missing groups...\n");
        H5_ERR_CHECK(H5Pset_create_intermediate_group(h5lcprop, 1));
        h5group = H5Gcreate(h5file, "/agents", h5lcprop, H5P_DEFAULT, H5P_DEFAULT);
        H5_ERR_CHECK(H5Gclose(h5group));

        h5_Circle_datatype = h5make_Circle_datatype();


        /*if(FLAME_integer_in_array(0, output_types, output_type_size))
        {
          printf("Creating Attribute: /states/itno: %i", iteration_number);
          H5_ERR_CHECK(H5LTset_attribute_int(h5file, "/states", "itno", &iteration_number, 1));
          printf("Write Environment Data to File: %s",file_name.c_str());
          h5write_environment(h5file);
        }*/

        printf("Write Agent Data to File: %s\n", file_name.c_str());
        h5write_Circle(h5file);

        H5_ERR_CHECK(H5Fclose(h5file));
        printf("File Closed: %s\n", file_name.c_str());
    }
    //! Write out an agent variable for all agents
    void writePop(std::string const& agent_name,
        std::string const& var_name, size_t size, void * ptr) {

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
