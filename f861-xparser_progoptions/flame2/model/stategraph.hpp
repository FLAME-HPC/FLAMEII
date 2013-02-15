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

namespace flame { namespace model {

/* \brief Define graph type
 *
 * Vectors are used for vertex and edge containers.
 * Bidirectional graph used for access to boost::in_edges
 * as well as boost::out_edges.
 */
typedef boost::adjacency_list
        <boost::vecS, boost::vecS, boost::bidirectionalS> Graph;
//! \brief Define vertex descriptor type
typedef boost::graph_traits<Graph>::vertex_descriptor Vertex;
//! \brief Define edge descriptor type
typedef boost::graph_traits<Graph>::edge_descriptor Edge;
//! \brief Define vertex iterator
typedef boost::graph_traits<Graph>::vertex_iterator VertexIterator;
//! \brief Define edge iterator
typedef boost::graph_traits<Graph>::edge_iterator EdgeIterator;
//! \brief Define edge mapping
typedef std::map<Edge, Dependency *> EdgeMap;

//! Use a shared pointer to automatically handle Task pointers
typedef boost::shared_ptr<Task> TaskPtr;

typedef std::set< std::pair<std::string, std::string> > StringPairSet;
typedef std::set<std::string> StringSet;
typedef size_t TaskId;
typedef std::set<TaskId> TaskIdSet;
typedef std::map<TaskId, TaskId> TaskIdMap;

class StateGraph {
  public:
    StateGraph();
    ~StateGraph();
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
    void setAgentName(std::string agentName);
    void importStateGraphTasks(StateGraph * graph,
        std::map<std::string, Vertex> * message2task,
        std::map<Vertex, Vertex> * import2new);
    std::vector<TaskPtr> * getVertexTaskMap();
    EdgeMap * getEdgeDependencyMap();
    Graph * getGraph() { return graph_; }
    void writeGraphviz(const std::string& fileName) const;
    void importStateGraphs(std::set<StateGraph*> graphs);
    Task * getStartTask();
    std::set<Task*> getEndTasks();

  private:
    /*! \brief Ptr to a graph so that graphs can be swapped */
    Graph * graph_;
    /*! \brief Ptr to vertex task so that mappings can be swapped */
    std::vector<TaskPtr> * vertex2task_;
    EdgeMap * edge2dependency_;
    Task * startTask_;
    std::set<Task *> endTasks_;
    std::string agentName_;

    Vertex addVertex(Task * t);
    Vertex addVertex(TaskPtr ptr);
    Edge addEdge(Vertex to, Vertex from, std::string name,
            Dependency::DependencyType type);
    Task * generateStateGraphStatesAddStateToGraph(
            std::string name, std::string startState);
    void generateStateGraphStates(XFunction * function, Task * task,
            std::string startState);
    void generateStateGraphVariables(XFunction * function, Task * task);
    Task * generateStateGraphMessagesAddMessageToGraph(std::string name);
    void generateStateGraphMessages(XFunction * function, Task * task);
    void setStartTask(Task * task);
    Vertex getVertex(Task * t);
    Task * getTask(Vertex v) const;
    Dependency * getDependency(Edge e);
    void removeVertex(Vertex v);
    void removeVertices(std::vector<Vertex> * tasks);
    void removeDependency(Edge e);
};

}}  // namespace flame::model
#endif  // MODEL__STATEGRAPH_HPP_
