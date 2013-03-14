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
#include <sqlite3.h>
#include <sstream>
#include <string>
#include <stdexcept>
#include "../io_interface.hpp"

// #include "flame2/config.hpp"  // Needed?

namespace flame { namespace io {

class IOSQLitePop : public IO {
    std::string getName() {
      return "sqlite";
    }
    //! Reading method, called by io manager
    void readPop(std::string path,
        void (*addInt)(std::string const&, std::string const&, int),
        void (*addDouble)(std::string const&, std::string const&, double)) {
      int rc;
      // open sqlite file
      rc = sqlite3_open(path.c_str(), &db);
      if (rc) {
        fprintf(stderr, "Can't open database: %s\n", sqlite3_errmsg(db));
        sqlite3_close(db);
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
        sqlite3_prepare(db, statement.c_str(), statement.length()+1, &selectStmt, NULL);
        int row = 0;
        int s;
        while(1) {
          s = sqlite3_step (selectStmt);
          if (s == SQLITE_ROW) {
          const unsigned char * text;
          text = sqlite3_column_text(selectStmt, 0);
          // for each variable
          VarVec::iterator vit;
          int column = 1;
          for (vit = ait->second.begin(); vit != ait->second.end(); ++vit) {
            text = sqlite3_column_text(selectStmt, column);
            if (vit->first == "int") addInt(ait->first, vit->second, atoi(reinterpret_cast<const char*>(text)));
            if (vit->first == "double") addDouble(ait->first, vit->second, atof(reinterpret_cast<const char*>(text)));
            ++column;
          }
          row++;
          } else if (s == SQLITE_DONE) {
            break;
          } else throw std::runtime_error("SQLite statement failed");
        }
      }
    }

    static int callback(
        void *NotUsed, int argc, char **argv, char **azColName) {
      int i;
      for (i = 0; i < argc; ++i) {
        printf("%s = %s\n", azColName[i], argv[i] ? argv[i] : "NULL");
      }
      printf("\n");
      return 0;
    }

    void executeSQLite(std::string statement) {
      char *zErrMsg = 0;
      int rc = sqlite3_exec(db, statement.c_str(), callback, 0, &zErrMsg);
      if (rc != SQLITE_OK) {
        fprintf(stderr, "SQL error: %s\n", zErrMsg);
        sqlite3_free(zErrMsg);
        throw std::runtime_error("SQL error on SQLite database");
      }
    }

    //! Initialise writing out of data for an iteration
    void initialiseData() {
      int rc;

      file_name_ = path_;
      std::ostringstream convert;
      convert << iteration_;
      file_name_.append(convert.str());
      file_name_.append(".sqlite");

      // delete any existing sqlite file
      remove(file_name_.c_str());
      // open sqlite file
      rc = sqlite3_open(file_name_.c_str(), &db);
      if (rc) {
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
        statement.append(" INTEGER PRIMARY KEY, ");
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
    //! Write out an agent variable for all agents
    void writePop(std::string const& agent_name,
        std::string const& var_name, size_t size, void * ptr) {
      int rc;
      size_t ii;
      // get index for agent table
      std::string index_name = index_name_map.find(agent_name)->second;

      // open sqlite file
      rc = sqlite3_open(file_name_.c_str(), &db);
      if (rc) {
        fprintf(stderr, "Can't open database: %s\n", sqlite3_errmsg(db));
        sqlite3_close(db);
        throw std::runtime_error("Can't open SQLite database");
      }

      // query table size and if zero then add rows for each agent
      std::string statement = "SELECT Count(*) FROM ";
      statement.append(agent_name);
      statement.append(";");
      sqlite3_stmt *selectStmt;
      sqlite3_prepare(db, statement.c_str(), statement.length()+1, &selectStmt, NULL);
      int num_rows;
      int s;
      while(1) {
        s = sqlite3_step (selectStmt);
        if (s == SQLITE_ROW) {
        const unsigned char * text;
        text  = sqlite3_column_text(selectStmt, 0);
        num_rows = atoi(reinterpret_cast<const char*>(text));
        //printf ("%d\n", num_rows);
        } else if (s == SQLITE_DONE) {
          break;
        } else throw std::runtime_error("SQLite statement failed");
      }
      // if table rows less than array size then add rows
      if (num_rows < size) {
        for (ii = num_rows; ii < size; ++ii) {
          std::string statement = "INSERT INTO ";
          statement.append(agent_name);
          statement.append("(");
          statement.append(index_name);
          statement.append(") VALUES (");
          std::ostringstream convert;
          convert << ii;
          statement.append(convert.str());
          statement.append(");");
          //printf("%s\n", statement.c_str());
          // execute insert
          executeSQLite(statement);
        }
      }

      // find var type
      // for each variable
      // for each agent type
      std::string var_type;
      AgentMemory::iterator ait;
      VarVec::iterator vit;
      for (ait = agentMemory_.begin(); ait != agentMemory_.end(); ++ait)
        if (ait->first == agent_name)
          for (vit = ait->second.begin(); vit != ait->second.end(); ++vit)
            if (vit->second == var_name) var_type = vit->first;

      // for each agent instance
      for (ii = 0; ii < size; ++ii) {
        // write data to database
        std::string statement = "UPDATE ";
        statement.append(agent_name);
        statement.append(" SET ");
        statement.append(var_name);
        statement.append("=");
        std::ostringstream convert;
        if (var_type == "int") convert << *(static_cast<int*>(ptr)+ii);
        if (var_type == "double") convert << *(static_cast<double*>(ptr)+ii);
        statement.append(convert.str());
        statement.append(" WHERE ");
        statement.append(index_name);
        statement.append("=");
        convert.str(std::string());  // clear stream to be used again
        convert << ii;
        statement.append(convert.str());
        statement.append(";");
        // execute insert
        //printf("%s\n", statement.c_str());
        executeSQLite(statement);
      }
      sqlite3_close(db);
    }
    //! Write out agents to file
    void writeAgents(FILE * fp) {
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
