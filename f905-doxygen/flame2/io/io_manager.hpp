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
#include <set>
#include <utility>
#include "flame2/exceptions/io.hpp"
#include "io_xml_model.hpp"
#include "io_interface.hpp"

namespace flame { namespace io {

typedef std::pair<std::string, std::string> Var;
typedef std::vector<Var> VarVec;
typedef std::map<std::string, VarVec> AgentMemory;

class IOManager {
  public:
    static IOManager& GetInstance() {
      static IOManager instance;
      return instance;
    }

    ~IOManager();

    //! Set input type
    void setInputType(std::string const& inputType);
    //! Set output type
    void setOutputType(std::string const& outputType);

    void loadModel(std::string const& file, flame::model::XModel * model);
    //! Called by sim
    //! \return iteration of pop file given by file name
    size_t readPop(std::string const& file_name);

    void setIteration(size_t i);
    void setAgentMemoryInfo(AgentMemory agentMemory);

    // Called by io tasks
    void writePop(std::string const& agent_name, std::string const& var_name);
    void initialiseData();
    void finaliseData();

#ifdef TESTBUILD
    IO * getIOPlugin(std::string const& name);
    //! Delete all input and output types
    void Reset();
#endif

  private:
    //! Information about agents, their names and variables (types and names)
    AgentMemory agentMemory_;
    //! Model XML reader
    xml::IOXMLModel ioxmlmodel_;
    //! Path to directory holding population files
    std::string path_;
    //! Map from plugin name to plugin
    std::map<std::string, IO*> plugins_;
    //! The current iteration number
    size_t iteration_;
    //! Input plugin
    IO * inputPlugin_;
    //! Output plugin
    IO * outputPlugin_;

    //! This is a singleton class. Disable manual instantiation
    IOManager();
    //! This is a singleton class. Disable copy constructor
    IOManager(const IOManager&);
    //! This is a singleton class. Disable assignment operation
    void operator=(const IOManager&);
};
}}  // namespace flame::io
#endif  // IO__IO_MANAGER_HPP_
