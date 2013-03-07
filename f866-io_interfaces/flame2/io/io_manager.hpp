/*!
void validateDatavoid validateData * \file flame2/io/io_manager.hpp
 * \author Simon Coakley
 * \date 2012
 * \copyright Copyright (c) 2012 STFC Rutherford Appleton Laboratory
 * \copyright Copyright (c) 2012 University of Sheffield
 * \copyright GNU Lesser General Public License
 * \brief IOManager: management for I/O Backend
 */
#ifndef IO__IO_MANAGER_HPP_
#define IO__IO_MANAGER_HPP_
#include <string>
#include <vector>
#include <map>
#include <utility>
#include "flame2/exceptions/io.hpp"
#include "io_xml_model.hpp"
#include "io_xml_pop.hpp"

namespace flame { namespace io {

typedef std::pair<std::string, std::string> Var;
typedef std::vector<Var> VarVec;
typedef std::map<std::string, VarVec> AgentMemory;

class IOManager {
  public:
    enum FileType { xml = 0 };

    static IOManager& GetInstance() {
      static IOManager instance;
      return instance;
    }

    void loadModel(std::string const& file, flame::model::XModel * model);
    void readPop(std::string const& file_name, FileType fileType);
    void writePop(std::string const& agent_name, std::string const& var_name);
    void initialiseData();
    void finaliseData();
    void setIteration(size_t i);
    void setAgentMemoryInfo(AgentMemory agentMemory);

  private:
    //! Information about agents, their names and variables (types and names)
    AgentMemory agentMemory_;
    //! Model XML reader
    xml::IOXMLModel ioxmlmodel_;
    //! Pop XML IO
    IOXMLPop   ioxmlpop_;
    //! The current iteration number
    size_t iteration_;

    //! This is a singleton class. Disable manual instantiation
    IOManager() : iteration_(0) {}
    //! This is a singleton class. Disable copy constructor
    IOManager(const IOManager&);
    //! This is a singleton class. Disable assignment operation
    void operator=(const IOManager&);
};
}}  // namespace flame::io
#endif  // IO__IO_MANAGER_HPP_
