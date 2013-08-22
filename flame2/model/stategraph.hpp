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

/*!
 * \brief Class to hold a state graph
 */
class StateGraph {
  public:
    /*!
     * \brief Constructor
     */
    StateGraph();
    /*!
     * \brief Generate state graph from agent functions
     */
    void generateStateGraph(boost::ptr_vector<XFunction> * functions,
            std::string startState, std::set<std::string> endStates);
    /*!
     * \brief Checks for cyclic dependencies within a graph
     * \return first integer for number of errors,
     *         second string for error message
     */
    std::pair<int, std::string> checkCyclicDependencies();
    /*!
     * \brief Checks for conditions on functions from a
     *        state with more than one out edge
     * \return first integer for number of errors,
     *         second string for error message
     */
    std::pair<int, std::string> checkFunctionConditions();
    /*!
     * \brief set graph name (agent name or model name)
     * \param[in] name Agent or model name
     */
    void setName(std::string name);
    /*!
     * \brief Get the edge dependnency map
     * \return Edge dependency map
     */
    EdgeMap * getEdgeDependencyMap();
    /*!
     * \brief Write out graph dot file
     * \param[in] fileName The path to the file to write out to
     */
    void writeGraphviz(const std::string& fileName) const;
    /*!
     * \brief Import set of state graphs and combine
     * \param[in] graphs Set of state graphs to import
     */
    void importStateGraphs(std::set<StateGraph*> graphs);
    /*!
     * \brief Get the source vertex of an edge
     * \param[in] e The edge
     * \return Vertex source of edge
     */
    Vertex getEdgeSource(Edge e);
    /*!
     * \brief Get the target vertex of an edge
     * \param[in] e The edge
     * \return Vertex target of edge
     */
    Vertex getEdgeTarget(Edge e);
    /*!
     * \brief Get graph edges
     * \return Edge iterators to iterate all edges
     */
    std::pair<EdgeIterator, EdgeIterator> getEdges();
    /*!
     * \brief Get the task list
     * \return List of all tasks
     */
    const TaskList * getTaskList() const;

  private:
    //! \brief Underlying graph
    XGraph graph_;
    //! \brief Name of graph (agent or model name)
    std::string name_;

    /*!
     * \brief Add state to graph and return associated task
     * \param[in] name State name
     * \param[in] startState The start state name
     * \return The associated task
     *
     * If the state has already been added its task is returned
     */
    ModelTask * generateStateGraphStatesAddStateToGraph(
            std::string name, std::string startState);
    /*!
     * \brief Add current and next state task for a function task
     * \param[in] function The function
     * \param[in] task The associated task
     * \param[in] startState The start state name
     */
    void generateStateGraphStates(XFunction * function, ModelTask * task,
            std::string startState);
    /*!
     * \brief Add read write variables to the function task
     * \param[in] function The function
     * \param[in] task The associated task
     */
    void generateStateGraphVariables(XFunction * function, ModelTask * task);
    /*!
     * \brief Add message to graph and return associated task
     * \param[in] name The message name
     * \return The associated task
     */
    ModelTask * generateStateGraphMessagesAddMessageToGraph(std::string name);
    /*!
     * \brief Add input and output edges to graph
     * \param[in] function The functions
     * \param[in] task The associated task
     */
    void generateStateGraphMessages(XFunction * function, ModelTask * task);
    /*!
     * \brief Add tasks from another state graph
     * \param[in] graph The state graph to import
     * \param[in,out] message2task The current graph message task map
     * \param[out] import2new Map between imported and new tasks
     */
    void importStateGraphTasks(StateGraph * graph,
            std::map<std::string, Vertex> * message2task,
            std::map<Vertex, Vertex> * import2new);
};

}}  // namespace flame::model
#endif  // MODEL__STATEGRAPH_HPP_
