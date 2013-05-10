//! \brief Namespace used by all FLAME-II components:
//!
//! \todo Refactor namespaces so all fall under the following parent categories 
//!    * `flame::core` - All more modules. Should not be accessed by end-users.
//!    * `flame::interfaces` - Interfaces that are common to core modules and 
//!       implementable by users' custom classes.
//!    * `flame:``:api` - APIs exposed to end users
//!    * `flame::contrib` - Reusable components contributed by users. Not officiall
//!       supported by the FLAME project.
//! 
namespace flame {

//! \brief API Module
//!
//! Contains interface classes used to build end-user APIs
namespace api {}

//! \brief EXE Module
//!
//! Classes for handling tasks scheduling and execution
namespace exe{}

//! \brief IO Module
//!
//! Classes for handling reading and writing of model description and population data.
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

}