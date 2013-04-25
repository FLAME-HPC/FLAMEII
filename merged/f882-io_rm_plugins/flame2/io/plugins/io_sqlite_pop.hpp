/*!
 * \file flame2/io/plugins/io_sqlite_pop.hpp
 * \author Simon Coakley
 * \date 2013
 * \copyright Copyright (c) 2013 STFC Rutherford Appleton Laboratory
 * \copyright Copyright (c) 2013 University of Sheffield
 * \copyright GNU Lesser General Public License
 * \brief IOSQLitePop: writing of population to SQLite file
 */
#ifdef HAVE_SQLITE3
#include <stdio.h>
#include <cstdlib>
#include <sstream>
#include <string>
#include <map>
#include <stdexcept>
#include "../io_interface.hpp"
#include "./sqlite3.h"

namespace flame { namespace io {

class IOSQLitePop : public IO {
    //! Return the plugin name
    std::string getName();
    //! read a column from a database
    void readColumn(AgentMemory::iterator ait, sqlite3_stmt *selectStmt,
        void (*addInt)(std::string const&, std::string const&, int),
        void (*addDouble)(std::string const&, std::string const&, double));
    //! Reading method, called by io manager
    void readPop(std::string path,
        void (*addInt)(std::string const&, std::string const&, int),
        void (*addDouble)(std::string const&, std::string const&, double));
    //! Execute SQLite statement
    void executeSQLite(std::string statement);
    //! Set file name using given path and iteration number
    void setFileName();
    //! Initialise writing out of data for an iteration
    void initialiseData();
    //! query table size and expand if needed
    void checkTableSize(std::string const& agent_name, size_t size,
        std::string const& index_name);
    //! Create insert statement for writePop
    std::string createUpdateStatement(std::string const& agent_name,
        std::string const& var_name, std::string const& index_name);
    //! Write out an agent variable for all agents
    void writePop(std::string const& agent_name,
        std::string const& var_name, size_t size, void * ptr);
    //! Finalise writing out of data for an iteration
    void finaliseData();

  private:
    //! The SQLite database
    sqlite3 *db;
    //! Path the the database file
    std::string file_name_;
    //! The index var name for each agent type
    std::map<std::string, std::string> index_name_map;
};

}}  // namespace flame::io
#endif
