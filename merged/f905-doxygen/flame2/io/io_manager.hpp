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
#include <boost/ptr_container/ptr_map.hpp>
#include "flame2/exceptions/io.hpp"
#include "io_xml_model.hpp"
#include "io_interface.hpp"

namespace flame { namespace io {

typedef std::pair<std::string, std::string> Var;
typedef std::vector<Var> VarVec;
typedef std::map<std::string, VarVec> AgentMemory;

/*!
 * \brief Manages input and output of flame2
 *
 * This class acts as a singleton and is used to: load models; set input and
 * output plugins; and read and write population data.
 */
class IOManager {
  public:
    //! Get an instance of the IOManager
    static IOManager& GetInstance() {
      static IOManager instance;
      return instance;
    }

    //! Set input type
    void setInputType(std::string const& inputType);
    //! Set output type
    void setOutputType(std::string const& outputType);
    /*!
     * \brief Load a model
     *
     * \param[in] file The path to the file
     * \param[out] model Pointer to the loaded model
     */
    void loadModel(std::string const& file, flame::model::XModel * model);
    /*!
     * \brief Read a population file
     *
     * \param[in] file_name The path to the population file
     * \return iteration number given by pop file name
     */
    size_t readPop(std::string const& file_name);
    /*!
     * \brief Set the iteration number
     *
     * Set the iteration number so that output files are correctly numbered
     *
     * \param[in] i The iteration number
     */
    void setIteration(size_t i);
    //! Set the agent memory information
    void setAgentMemoryInfo(AgentMemory agentMemory);

    //! Write agent variable array (called by io tasks)
    void writePop(std::string const& agent_name, std::string const& var_name);
    //! Initialise writing of data (called by io task)
    void initialiseData();
    //! Finalise writing of data (called by io task)
    void finaliseData();

#ifdef TESTBUILD
    //! Return a pointer to the IO plugin named
    IOInterface * getIOPlugin(std::string const& name);
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
    boost::ptr_map<std::string, IOInterface> plugins_;
    //! The current iteration number
    size_t iteration_;
    //! Input plugin
    IOInterface * inputPlugin_;
    //! Output plugin
    IOInterface * outputPlugin_;

    //! This is a singleton class. Disable manual instantiation
    IOManager();
    //! This is a singleton class. Disable copy constructor
    IOManager(const IOManager&);
    //! This is a singleton class. Disable assignment operation
    void operator=(const IOManager&);
};
}}  // namespace flame::io
#endif  // IO__IO_MANAGER_HPP_
