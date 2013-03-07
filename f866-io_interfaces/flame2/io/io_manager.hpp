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
    static IOManager& GetInstance() {
      static IOManager instance;
      return instance;
    }

    //! Register input and output plugins
    void registerIOPlugins();

    //! Add input type
    void addInputType(std::string const& inputType);
    //! Set input type
    void setInputType(std::string const& inputType);
    //! Add output type
    void addOutputType(std::string const& outputType);
    //! Set output type
    void setOutputType(std::string const& outputType);

    void loadModel(std::string const& file, flame::model::XModel * model);
    void readPop(std::string const& file_name);
    void setIteration(size_t i);
    void setAgentMemoryInfo(AgentMemory agentMemory);

    // Called by io tasks
    void writePop(std::string const& agent_name, std::string const& var_name);
    void initialiseData();
    void finaliseData();

#ifdef TESTBUILD
    //! Delete all input and output types
    void Reset();
#endif

  private:
    //! Information about agents, their names and variables (types and names)
    AgentMemory agentMemory_;
    //! Model XML reader
    xml::IOXMLModel ioxmlmodel_;
    //! Pop XML IO
    IOXMLPop ioxmlpop_;
    //! The current iteration number
    size_t iteration_;
    //! Set of input types
    std::set<std::string> inputTypes_;
    //! Set of output types
    std::set<std::string> outputTypes_;
    //! Input type
    std::string inputType_;
    //! Output type
    std::string outputType_;

    //! This is a singleton class. Disable manual instantiation
    IOManager() : iteration_(0) {}
    //! This is a singleton class. Disable copy constructor
    IOManager(const IOManager&);
    //! This is a singleton class. Disable assignment operation
    void operator=(const IOManager&);
};
}}  // namespace flame::io
#endif  // IO__IO_MANAGER_HPP_
