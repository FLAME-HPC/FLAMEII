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

/*!
 * \brief IO Plugin to read and write to SQLite files
 */
class IOSQLitePop : public IOInterface {
  public:
    /*!
     * \brief Constructor
     */
    IOSQLitePop() : db(0), file_name_(), index_name_map() {}
    /*!
     * \brief Return plugin name
     * \return Plugin name
     */
    std::string getName();
    /*!
     * \brief Read SQLite population file
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
     * \brief Write agent variable array to XML file
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
    //! \brief The SQLite database
    sqlite3 *db;
    //! \brief Path the the database file
    std::string file_name_;
    //! \brief The index var name for each agent type
    std::map<std::string, std::string> index_name_map;
    /*!
     * \brief Read a column from a database
     * \param[in] ait Agent memory iterator
     * \param[in] selectStmt SQL select statement
     * \param[out] addInt Function to add integers
     * \param[out] addDouble Function to add doubles
     */
    void readColumn(AgentMemory::iterator ait, sqlite3_stmt *selectStmt,
        void (*addInt)(std::string const&, std::string const&, int),
        void (*addDouble)(std::string const&, std::string const&, double));
    /*!
     * \brief Execute SQLite statement
     * \param[in] statement SQL statement to execute
     */
    void executeSQLite(std::string statement);
    //! \brief Set file name using given path and iteration number
    void setFileName();
    /*!
     * \brief Query table size and expand if needed
     * \param[in] agent_name Agent name
     * \param[in] size Size to check from
     * \param[in] index_name Index
     */
    void checkTableSize(std::string const& agent_name, size_t size,
        std::string const& index_name);
    /*!
     * \brief Create insert statement for writePop
     * \param[in] agent_name Agent name
     * \param[in] var_name Variable name
     * \param[in] index_name Index
     */
    std::string createUpdateStatement(std::string const& agent_name,
        std::string const& var_name, std::string const& index_name);
};

}}  // namespace flame::io
#endif
