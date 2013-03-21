/*!
 * \file flame2/io/plugins/io_sqlite_pop.cpp
 * \author Simon Coakley
 * \date 2013
 * \copyright Copyright (c) 2013 STFC Rutherford Appleton Laboratory
 * \copyright Copyright (c) 2013 University of Sheffield
 * \copyright GNU Lesser General Public License
 * \brief IOSQLitePop: writing of population to SQLite file
 */
#include <stdio.h>
#include <cstdlib>
#include <sstream>
#include <string>
#include <map>
#include <stdexcept>
#include "../io_interface.hpp"
#include "./sqlite3.h"

// #include "flame2/config.hpp"  // Needed?

namespace flame { namespace io {

class IOSQLitePop : public IO {
    //! Return the plugin name
    std::string getName() {
      return "sqlite";
    }
    //! read a column from a database
    void readColumn(AgentMemory::iterator ait, sqlite3_stmt *selectStmt,
        void (*addInt)(std::string const&, std::string const&, int),
        void (*addDouble)(std::string const&, std::string const&, double)) {
      int s;  // status code
      VarVec::iterator vit;
      // start loop
      while (1) {
        // step though the select statement results
        s = sqlite3_step(selectStmt);
        // if row
        if (s == SQLITE_ROW) {
          // read each column from 1 (column 0 is the index)
          int column = 1;
          // for each variable
          for (vit = ait->second.begin(); vit != ait->second.end(); ++vit) {
            // read the text of the column result
            const unsigned char * text =
                sqlite3_column_text(selectStmt, column);
            // if int then cast and add
            if (vit->first == "int") addInt(ait->first, vit->second,
                atoi(reinterpret_cast<const char*>(text)));
            // if double then cast and add
            if (vit->first == "double") addDouble(ait->first, vit->second,
                atof(reinterpret_cast<const char*>(text)));
            // increment the column for the next variable
            ++column;
          }
        // if finished then break from loop
        } else if (s == SQLITE_DONE) {
          break;
        // else if error then throw error
        } else {
          throw std::runtime_error("SQLite statement failed");
        }
      }
    }

    //! Reading method, called by io manager
    void readPop(std::string path,
        void (*addInt)(std::string const&, std::string const&, int),
        void (*addDouble)(std::string const&, std::string const&, double)) {
      // open sqlite file
      if (sqlite3_open(path.c_str(), &db) != SQLITE_OK) {
        fprintf(stderr, "Can't open database: %s\n", sqlite3_errmsg(db));
        throw std::runtime_error("Can't open SQLite database");
      }

      // read data in
      // for each agent type
      AgentMemory::iterator ait;
      for (ait = agentMemory_.begin(); ait != agentMemory_.end(); ++ait) {
        std::string statement = "SELECT * FROM ";
        statement.append(ait->first);
        statement.append(";");
        sqlite3_stmt *selectStmt;
        sqlite3_prepare(
            db, statement.c_str(), statement.length()+1, &selectStmt, NULL);
        readColumn(ait, selectStmt, addInt, addDouble);
        // delete the statement
        sqlite3_finalize(selectStmt);
      }

      // close db
      sqlite3_close(db);
    }
    //! SQLite call back function
    static int callback(
        void */*NotUsed*/, int argc, char **argv, char **azColName) {
      int i;
      for (i = 0; i < argc; ++i) {
        printf("%s = %s\n", azColName[i], argv[i] ? argv[i] : "NULL");
      }
      printf("\n");
      return 0;
    }
    //! Execute SQLite statement
    void executeSQLite(std::string statement) {
      char *zErrMsg = 0;
      int rc = sqlite3_exec(db, statement.c_str(), callback, 0, &zErrMsg);
      if (rc != SQLITE_OK) {
        fprintf(stderr, "SQL error: %s\n", zErrMsg);
        sqlite3_free(zErrMsg);
        throw std::runtime_error("SQL error on SQLite database");
      }
    }
    //! Set file name using given path and iteration number
    void setFileName() {
      file_name_ = path_;
      std::ostringstream convert;
      convert << iteration_;
      file_name_.append(convert.str());
      file_name_.append(".sqlite");
    }
    //! Initialise writing out of data for an iteration
    void initialiseData() {
      // set file name using given path and iteration number
      setFileName();

      // delete any existing sqlite file
      remove(file_name_.c_str());
      // open sqlite file
      if (sqlite3_open(file_name_.c_str(), &db) != SQLITE_OK) {
        fprintf(stderr, "Can't open database: %s\n", sqlite3_errmsg(db));
        sqlite3_close(db);
        throw std::runtime_error("Can't open SQLite database");
      }
      // for each agent type
      AgentMemory::iterator ait;
      for (ait = agentMemory_.begin(); ait != agentMemory_.end(); ++ait) {
        // set index name ToDo make sure there are no clashes
        std::string index_name = "flame_index";
        index_name_map.insert(std::make_pair(ait->first, index_name));
        //  create a database
        std::string statement = "CREATE TABLE ";
        statement.append(ait->first);  // agent name
        statement.append("(");
        statement.append(index_name);
        statement.append(" INTEGER, ");
        // for each variable
        VarVec::iterator vit;
        for (vit = ait->second.begin(); vit != ait->second.end();) {
          statement.append(vit->second);  // var name
          if (vit->first == "int") statement.append(" INTEGER");
          if (vit->first == "double") statement.append(" DOUBLE");
          if (++vit != ait->second.end()) statement.append(", ");
        }
        statement.append(");");
        // create table for agent type
        executeSQLite(statement);
      }
      sqlite3_close(db);
    }
    //! query table size and expand if needed
    void checkTableSize(std::string const& agent_name, size_t size,
        std::string const& index_name) {
      // create the select statement to get the table size
      std::string statement = "SELECT Count(*) FROM ";
      statement.append(agent_name).append(";");
      // the statement
      sqlite3_stmt *selectStmt;
      // prepare the statement
      sqlite3_prepare(
          db, statement.c_str(), statement.length()+1, &selectStmt, NULL);
      int s, num_rows;
      // loop
      while (1) {
        // execute statement
        s = sqlite3_step(selectStmt);
        // if row result
        if (s == SQLITE_ROW)
          // interpret result
          num_rows = atoi(reinterpret_cast<const char*>(
              sqlite3_column_text(selectStmt, 0)));
        else if (s == SQLITE_DONE)
          break;
        else
          throw std::runtime_error("SQLite statement failed");
      }
      // if table rows less than array size then add rows
      if ((size_t)(num_rows) < size) {
        // for ech new row
        for (size_t ii = num_rows; ii < size; ++ii) {
          // create insert row statement
          statement = "INSERT INTO ";
          // add agent name
          statement.append(agent_name).append("(");
          // add index var with index
          statement.append(index_name).append(") VALUES (");
          std::ostringstream convert;
          convert << ii;
          statement.append(convert.str()).append(");");
          // execute insert
          executeSQLite(statement);
        }
      }
      // delete the statement
      sqlite3_finalize(selectStmt);
    }
    //! Create insert statement for writePop
    std::string createUpdateStatement(std::string const& agent_name,
        std::string const& var_name, std::string const& index_name) {
      std::string statement = "UPDATE ";
      statement.append(agent_name).append(" SET ");
      statement.append(var_name).append("=? WHERE ");
      statement.append(index_name).append("=?");
      return statement;
    }
    //! Write out an agent variable for all agents
    void writePop(std::string const& agent_name,
        std::string const& var_name, size_t size, void * ptr) {
      char *sErrMsg = 0;

      // open sqlite file
      if (sqlite3_open(file_name_.c_str(), &db) != SQLITE_OK) {
        fprintf(stderr, "Can't open database: %s\n", sqlite3_errmsg(db));
        throw std::runtime_error("Can't open SQLite database");
      }

      // get index for agent table
      std::string index_name = index_name_map.find(agent_name)->second;

      // turn off blocking of write statements
      // sqlite3_exec(db, "PRAGMA synchronous = OFF", NULL, NULL, &sErrMsg);

      // wrap updates in a transaction
      sqlite3_exec(db, "BEGIN TRANSACTION", NULL, NULL, &sErrMsg);

      // query table size and expand if needed
      checkTableSize(agent_name, size, index_name);

      // find var type
      std::string var_type = getVariableType(agent_name, var_name);

      // use prepared statement
      std::string s = createUpdateStatement(agent_name, var_name, index_name);
      int buffer_size = s.size()+1;
      const char * sSQL = s.c_str();
      sqlite3_stmt * stmt;
      const char * tail = 0;
      sqlite3_prepare_v2(db, sSQL, buffer_size, &stmt, &tail);

      // for each agent instance
      for (size_t ii = 0; ii < size; ++ii) {
        // write data to database
        if (var_type == "int") sqlite3_bind_int(
            stmt, 1, *(static_cast<int*>(ptr)+ii));
        if (var_type == "double") sqlite3_bind_double(
            stmt, 1, *(static_cast<double*>(ptr)+ii));
        sqlite3_bind_int(stmt, 2, ii);
        sqlite3_step(stmt);  // commit
        sqlite3_clear_bindings(stmt);
        sqlite3_reset(stmt);
      }
      // delete the statement
      sqlite3_finalize(stmt);

      // close transaction and execute
      sqlite3_exec(db, "END TRANSACTION", NULL, NULL, &sErrMsg);

      sqlite3_close(db);
    }
    //! Finalise writing out of data for an iteration
    void finaliseData() {
    }

  private:
    //! The SQLite database
    sqlite3 *db;
    //! Path the the database file
    std::string file_name_;
    //! The index var name for each agent type
    std::map<std::string, std::string> index_name_map;
};

// set 'C' linkage for function names
extern "C" {
  // function to return an instance of a new IO plugin object
  IO* getIOPlugin() {
    return new IOSQLitePop();
  }
}
}}  // namespace flame::io
