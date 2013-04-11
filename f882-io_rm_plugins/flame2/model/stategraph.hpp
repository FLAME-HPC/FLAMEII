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
#include "model_task.hpp"
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
    //! \return edge dependency map
    EdgeMap * getEdgeDependencyMap();
    //! write out graph dot file
    void writeGraphviz(const std::string& fileName) const;
    //! import set of state graphs and combine
    void importStateGraphs(std::set<StateGraph*> graphs);
    //! \return Vertex source of edge
    Vertex getEdgeSource(Edge e);
    //! \return Vertex target of edge
    Vertex getEdgeTarget(Edge e);
    //! \return Edge iterators to iterate all edges
    std::pair<EdgeIterator, EdgeIterator> getEdges();
    //! \return List of all tasks
    const TaskList * getTaskList() const;

  private:
    //! Underlying graph
    XGraph graph_;
    //! Name of graph (agent or model name)
    std::string name_;

    //! Add state to graph or return task if already added
    ModelTask * generateStateGraphStatesAddStateToGraph(
            std::string name, std::string startState);
    //! Add current and next state task for a function task
    void generateStateGraphStates(XFunction * function, ModelTask * task,
            std::string startState);
    //! Add read write variables to the function task
    void generateStateGraphVariables(XFunction * function, ModelTask * task);
    //! Add message to graph or return task if already added
    ModelTask * generateStateGraphMessagesAddMessageToGraph(std::string name);
    //! Add input and output edges to graph
    void generateStateGraphMessages(XFunction * function, ModelTask * task);
    //! Add tasks from another state graph
    void importStateGraphTasks(StateGraph * graph,
            std::map<std::string, Vertex> * message2task,
            std::map<Vertex, Vertex> * import2new);
};

}}  // namespace flame::model
#endif  // MODEL__STATEGRAPH_HPP_
