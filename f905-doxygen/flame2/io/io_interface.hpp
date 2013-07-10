/*!
 * \file flame2/io/io_interface.hpp
 * \author Simon Coakley
 * \date 2013
 * \copyright Copyright (c) 2013 STFC Rutherford Appleton Laboratory
 * \copyright Copyright (c) 2013 University of Sheffield
 * \copyright GNU Lesser General Public License
 * \brief Interface class for output types
 */
#ifndef IO__IO_INTERFACE_HPP_
#define IO__IO_INTERFACE_HPP_
#include <string>
#include <utility>
#include <map>
#include <vector>

namespace flame { namespace io {

//! A data type for holding model variable type and name
typedef std::pair<std::string, std::string> Var;
//! A data type for holding a vector of model variables
typedef std::vector<Var> VarVec;
//! A data type for holding agent memory variables
typedef std::map<std::string, VarVec> AgentMemory;

/*!
 * \brief Interface class for implementing IO plugins
 */
class IO {
  public:
    IO() : agentMemory_(), iteration_(0), path_() {}
    virtual ~IO() {}

    //! Get the name of the plugin
    virtual std::string getName() = 0;
    //! Reading method, called by io manager
    virtual void readPop(std::string path,
        void (*addInt)(std::string const&, std::string const&, int),
        void (*addDouble)(std::string const&, std::string const&, double)) = 0;
    //! Initialise writing out of data for an iteration
    virtual void initialiseData() = 0;
    //! Write out an agent variable for all agents
    virtual void writePop(std::string const& agent_name,
        std::string const& var_name, size_t size, void * ptr) = 0;
    //! Finalise writing out of data for an iteration
    virtual void finaliseData() = 0;
    //! Set agent memory info
    void setAgentMemoryInfo(AgentMemory agentMemory) {
        agentMemory_ = agentMemory;
    }
    //! Set the iteration number
    void setIteration(size_t i) {
      iteration_ = i;
    }
    //! Set path to write to
    void setPath(std::string path) {
      path_ = path;
    }
    // Need write agent for newly added agents? or just use writePop again?
    // virtual void writeAgent();

  protected:
    //! Data structure holding agent names and their variables names and types
    AgentMemory agentMemory_;
    //! The current iteration number
    size_t iteration_;
    //! The current path to write population data to
    std::string path_;

    //! Gets the type of the agent variable specified
    std::string getVariableType(
        std::string const& agent_name, std::string const& var_name) {
      AgentMemory::iterator ait;
      VarVec::iterator vit;
      for (ait = agentMemory_.begin(); ait != agentMemory_.end(); ++ait)
        if (ait->first == agent_name)
          for (vit = ait->second.begin(); vit != ait->second.end(); ++vit)
            if (vit->second == var_name) return vit->first;
      throw std::runtime_error("Agent variable does not exist");
}
};

}}  // namespace flame::io
#endif  // IO__IO_INTERFACE_HPP_
