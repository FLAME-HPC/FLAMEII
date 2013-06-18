/*!
 * \file docs/namespaces.hpp
 * \author Shawn Chin
 * \date 2012
 * \copyright Copyright (c) 2012 STFC Rutherford Appleton Laboratory
 * \copyright Copyright (c) 2012 University of Sheffield
 * \copyright GNU Lesser General Public License
 * \brief Namespace used by all FLAME-II components:
 *
 * \todo Refactor namespaces so all fall under the following parent categories
 *    * `flame::core` - All more modules. Should not be accessed by end-users.
 *    * `flame::interfaces` - Interfaces that are common to core modules and
 *       implementable by users' custom classes.
 *    * `flame:``:api` - APIs exposed to end users
 *    * `flame::contrib` - Reusable components contributed by users. Not officially
 *       supported by the FLAME project.
*/

//! \brief FLAME2
//!
//! Namespace used by libflame2
namespace flame {

//! \brief API Module
//!
//! Contains interface classes used to build end-user APIs
namespace api {}

//! \brief EXE Module
//!
//! Classes for handling tasks scheduling and execution
namespace exe {}

//! \brief IO Module
//!
//! Classes for handling reading and writing of model description and
//!     population data.
namespace io {}

//! \brief MB Module
//!
//! Classes for message handling
namespace mb {}

//! \brief MEM Module
//!
//! Classes for managing agent memory
namespace mem {}

//! \brief MODEL Module
//!
//! Classes for representing model definitions
namespace model {}

//! \brief SIM Module
//!
//! Classes for orchestrating a simulation
namespace sim {}

//! \brief Exceptions
//!
//! Exception subclasses that are thrown by FLAME2
namespace exceptions {}

}

//! \brief Xparser2
//!
//! Namespace used by xparser2
namespace xparser {

//! \brief Code generators
//!
//! Code generator classes
namespace codegen {}

}
