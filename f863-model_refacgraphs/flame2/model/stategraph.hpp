/*!
 * \file flame2/model/stategraph.hpp
 * \author Simon Coakley
 * \date 2012
 * \copyright Copyright (c) 2012 STFC Rutherford Appleton Laboratory
 * \copyright Copyright (c) 2012 University of Sheffield
 * \copyright GNU Lesser General Public License
 * \brief StateGraph: holds state graph
 */
#ifndef MODEL__STATEGRAPH_HPP_
#define MODEL__STATEGRAPH_HPP_
#include <boost/graph/adjacency_list.hpp>
#include <boost/property_map/property_map.hpp>
#include <boost/shared_ptr.hpp>
#include <vector>
#include <string>
#include <map>
#include <set>
#include <utility>  // for std::pair
#include "flame2/exe/task_manager.hpp"
#include "dependency.hpp"
#include "task.hpp"
#include "xfunction.hpp"
#include "xvariable.hpp"
#include "xgraph.hpp"

namespace flame { namespace model {

class StateGraph {
  public:
    //! Constructor
    StateGraph();
    //! Generate state graph from agent functions
    int generateStateGraph(boost::ptr_vector<XFunction> * functions,
            std::string startState, std::set<std::string> endStates);
    //! Checks for cyclic dependencies within a graph
    //! \return first integer for number of errors,
    //!         second string for error message
    std::pair<int, std::string> checkCyclicDependencies();
    //! Checks for conditions on functions from a state
    //! with more than one out edge
    //! \return first integer for number of errors,
    //!         second string for error message
    std::pair<int, std::string> checkFunctionConditions();
    //! set graph name (agent name or model name)
    void setName(std::string name);
    //! \return vertex task map
    std::vector<TaskPtr> * getVertexTaskMap();
    //! \return edge dependency map
    EdgeMap * getEdgeDependencyMap();
    //! write out graph dot file
    void writeGraphviz(const std::string& fileName) const;
    //! import set of state graphs and combine
    void importStateGraphs(std::set<StateGraph*> graphs);
    Vertex getEdgeSource(Edge e);
    Vertex getEdgeTarget(Edge e);
    std::pair<EdgeIterator, EdgeIterator> getEdges();

  private:
    //! Underlying graph
    XGraph graph_;
    //! Name of graph (agent or model name)
    std::string name_;

    Task * generateStateGraphStatesAddStateToGraph(
            std::string name, std::string startState);
    void generateStateGraphStates(XFunction * function, Task * task,
            std::string startState);
    void generateStateGraphVariables(XFunction * function, Task * task);
    Task * generateStateGraphMessagesAddMessageToGraph(std::string name);
    void generateStateGraphMessages(XFunction * function, Task * task);
    void importStateGraphTasks(StateGraph * graph,
            std::map<std::string, Vertex> * message2task,
            std::map<Vertex, Vertex> * import2new);
};

}}  // namespace flame::model
#endif  // MODEL__STATEGRAPH_HPP_
