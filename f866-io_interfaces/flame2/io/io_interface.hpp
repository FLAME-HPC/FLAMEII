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

typedef std::pair<std::string, std::string> Var;
typedef std::vector<Var> VarVec;
typedef std::map<std::string, VarVec> AgentMemory;

class IO {
  public:
    virtual ~IO() {}

    //! Reading method, called by io manager
    virtual void readPop(std::string path,
        void (*addInt)(std::string const&, std::string const&, int),
        void (*addDouble)(std::string const&, std::string const&, double)) = 0;
    //! Writing methods, called by exe::io tasks via io manager
    virtual void initialiseData() = 0;
    virtual void writePop(std::string const& agent_name,
        std::string const& var_name, size_t size, void * ptr) = 0;
    virtual void finaliseData() = 0;
    // Need write agent for newly added agents? or just use writePop again?
    // virtual void writeAgent();

    void setAgentMemoryInfo(AgentMemory agentMemory) {
      agentMemory_ = agentMemory;
    }

  protected:
    AgentMemory agentMemory_;
};

}}  // namespace flame::io
#endif  // IO__IO_INTERFACE_HPP_
