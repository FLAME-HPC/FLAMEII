/*!
 * \file src/model/xgraph.cpp
 * \author Simon Coakley
 * \date 2012
 * \copyright Copyright (c) 2012 STFC Rutherford Appleton Laboratory
 * \copyright Copyright (c) 2012 University of Sheffield
 * \copyright GNU Lesser General Public License
 * \brief XGraph: holds graph information
 */
#include <boost/graph/topological_sort.hpp>
#include <boost/graph/graphviz.hpp>
#include <boost/graph/graph_utility.hpp>
#include <boost/graph/transitive_reduction.hpp>
#include <boost/tuple/tuple.hpp>
#include <string>
#include <vector>
#include <set>
#include <map>
#include <utility>
#include <exception>
#include <functional>  // For greater<>
#include <algorithm>   // For sort
#include "./xgraph.hpp"
#include "./xcondition.hpp"
#include "./xfunction.hpp"
#include "./task.hpp"

namespace flame { namespace model {

XGraph::XGraph() {
    // Initialise pointers
    graph_ = new Graph;
    vertex2task_ = new std::vector<Task *>;
    edge2dependency_ = new EdgeMap;
}

XGraph::~XGraph() {
    /* Free task memory */
    std::vector<Task *>::iterator vit;
    for (vit = vertex2task_->begin(); vit != vertex2task_->end(); vit++)
        delete (*vit);
    // Free vertex task mapping
    delete vertex2task_;
    // Free edge dependency mapping
    delete edge2dependency_;
    // Free graph
    delete graph_;
}

void XGraph::setAgentName(std::string agentName) {
    agentName_ = agentName;
}

Vertex XGraph::addVertex(Task * t) {
    // Add vertex to graph
    Vertex v = add_vertex(*graph_);
    // Add task to vertex task mapping
    vertex2task_->push_back(t);
    // Return vertex
    return v;
}

Vertex XGraph::addVertex(std::string name, Task::TaskType type) {
    // Create task using name and type
    Task * task = new Task(name, type);
    // Add vertex using task
    return addVertex(task);
}

void XGraph::removeTask(Vertex v) {
    // Iterators
    boost::graph_traits<Graph>::out_edge_iterator oei, oei_end;
    boost::graph_traits<Graph>::in_edge_iterator iei, iei_end;
    std::set<Edge>::iterator eit;
    // Set of edges to remove
    std::set<Edge> edgesToRemove;
    // Add all in and out vertex edges to set of edges to be removed
    for (boost::tie(iei, iei_end) = boost::in_edges(v, *graph_);
            iei != iei_end; ++iei)
        edgesToRemove.insert((Edge)*iei);
    for (boost::tie(oei, oei_end) = boost::out_edges(v, *graph_);
            oei != oei_end; ++oei)
        edgesToRemove.insert((Edge)*oei);
    // Remove all edges in set of edges to be removed
    for (eit = edgesToRemove.begin(); eit != edgesToRemove.end(); eit++)
            removeDependency(*eit);
    // Free task associated with vertex
    delete vertex2task_->at(v);
    // Remove task from vertex to task mapping mapping
    vertex2task_->erase(vertex2task_->begin() + v);
    // Remove edge from graph
    boost::remove_vertex(v, *graph_);
}

// Tasks removed largest first so that indexes are not changed
void XGraph::removeTasks(std::vector<Vertex> * tasks) {
    std::vector<Vertex>::iterator vit;
    // Sort vertices largest first
    std::sort(tasks->begin(), tasks->end(), std::greater<size_t>());
    // Remove tasks in order
    for (vit = tasks->begin(); vit != tasks->end(); ++vit)
        removeTask((*vit));
}

void XGraph::removeDependency(Edge e) {
    // Find dependency
    EdgeMap::iterator it = edge2dependency_->find(e);
    // If found then free
    if (it != edge2dependency_->end()) {
        delete (*it).second;
        edge2dependency_->erase(it);
    }
    // Remove edge from graph
    boost::remove_edge(e, *graph_);
}

Edge XGraph::addEdge(Vertex to, Vertex from, Dependency * d) {
    // Add edge to graph
    std::pair<Edge, bool> e = add_edge(to, from, *graph_);
    // Add mapping from edge to dependency
    edge2dependency_->insert(std::make_pair(e.first, d));
    // Return edge
    return e.first;
}

Edge XGraph::addEdge(Vertex to, Vertex from, std::string name,
        Dependency::DependencyType type) {
    // Create dependency from name and type
    Dependency * d = new Dependency(name, type);
    // Add edge using dependency
    return addEdge(to, from, d);
}

void XGraph::addEdge(Task * to, Task * from, Dependency * d) {
    // Add edge by getting task vertices
    addEdge(getVertex(to), getVertex(from), d);
}

void XGraph::addEdge(Task * to, Task * from, std::string name,
        Dependency::DependencyType type) {
    // Create dependency from name and type
    Dependency * d = new Dependency(name, type);
    // Add edge using dependency
    addEdge(to, from, d);
}

Vertex XGraph::getVertex(Task * t) {
    size_t ii;
    // Find index of task in vertex task mapping
    // The index corresponds to the vertex number
    for (ii = 0; ii < vertex2task_->size(); ii++)
        if (vertex2task_->at(ii) == t) return ii;
    return 0;
}

Task * XGraph::getTask(Vertex v) {
    // Return task at index v
    return vertex2task_->at(v);
}

Dependency * XGraph::getDependency(Edge e) {
    // Return dependency associated with the edge
    return edge2dependency_->find(e)->second;
}

int XGraph::generateDependencyGraph(std::vector<XVariable*> * variables) {
#ifdef TESTBUILD
    writeGraphviz(agentName_ + "_1.dot");
#endif

    // Add condition vertices
    addConditionVertices();

    // Contract state vertices
    contractStateVertices();
#ifdef TESTBUILD
    writeGraphviz(agentName_ + "_2.dot");
#endif

    // Add variable vertices
    addVariableVerticesToGraph(variables);

    // Add condition dependencies
    addConditionDependencies();

    // Remove state dependencies
    removeStateDependencies();
#ifdef TESTBUILD
    writeGraphviz(agentName_ + "_3.dot");
#endif
    // Add data io
    AddVariableOutput(variables);
    // Contract variable vertices
    contractVariableVertices();
#ifdef TESTBUILD
    writeGraphviz(agentName_ + "_4.dot");
#endif
    // Remove redundant dependencies
    removeRedundantDependencies();
#ifdef TESTBUILD
    writeGraphviz(agentName_ + "_5.dot");
#endif
    return 0;
}

Task * XGraph::generateStateGraphStatesAddStateToGraph(std::string name,
        std::string startState) {
    // Check if state has already been added
    std::vector<Task *>::iterator vit;
    for (vit = vertex2task_->begin(); vit != vertex2task_->end(); ++vit)
        if ((*vit)->getTaskType() == Task::xstate &&
                (*vit)->getName() == name) return (*vit);

    // Add state as a task to the task list
    Task * task = new Task(name, Task::xstate);
    addVertex(task);
    if (name == startState) startTask_ = task;

    return task;
}

void XGraph::generateStateGraphStates(XFunction * function, Task * task,
        std::string startState) {
    std::set<std::string>::iterator sit;
    // Add and get state tasks
    Task * currentState =
            generateStateGraphStatesAddStateToGraph(
                    function->getCurrentState(), startState);
    Task * nextState =
            generateStateGraphStatesAddStateToGraph(
                    function->getNextState(), startState);
    // Add edge from current state to function
    addEdge(currentState, task,
            function->getCurrentState(), Dependency::state);
    // Add edge from function to next state
    addEdge(task, nextState,
            function->getNextState(), Dependency::state);
    // If transition has a condition
    if (function->getCondition()) {
        // Set task has a condition
        task->setHasCondition(true);
        // Get condition read variables
        std::set<std::string> * rov =
                function->getCondition()->getReadOnlyVariables();
        // For each condition read variable add to current state
        // read variables
        for (sit = rov->begin(); sit != rov->end(); sit++)
            currentState->addReadVariable(*sit);
    }
}

void XGraph::generateStateGraphVariables(XFunction * function, Task * task) {
    std::vector<std::string>::iterator sitv;
    // For each read only variable
    for (sitv = function->getReadOnlyVariables()->begin();
            sitv != function->getReadOnlyVariables()->end(); sitv++)
        // Add to task read variables
        task->addReadVariable(*sitv);
    // For each read write variable
    for (sitv = function->getReadWriteVariables()->begin();
            sitv != function->getReadWriteVariables()->end(); sitv++) {
        // Add to task read variables
        task->addReadVariable(*sitv);
        // Add to task write variables
        task->addWriteVariable(*sitv);
    }
}

Task * XGraph::generateStateGraphMessagesAddMessageToGraph(std::string name) {
    // Check if state has already been added
    std::vector<Task *>::iterator vit;
    for (vit = vertex2task_->begin(); vit != vertex2task_->end(); ++vit)
        if ((*vit)->getTaskType() == Task::xmessage &&
                (*vit)->getName() == name) return (*vit);

    // Add state as a task to the task list
    Task * task = new Task(name, Task::xmessage);
    addVertex(task);
    return task;
}

void XGraph::generateStateGraphMessages(XFunction * function, Task * task) {
    std::vector<XIOput*>::iterator ioput;
    // Find outputting functions
    for (ioput = function->getOutputs()->begin();
         ioput != function->getOutputs()->end(); ++ioput)
        // Add edge from function vertex to message vertex
        addEdge(task,
                generateStateGraphMessagesAddMessageToGraph(
                    (*ioput)->getMessageName()),
                (*ioput)->getMessageName(), Dependency::communication);
    // Find inputting functions
    for (ioput = function->getInputs()->begin();
         ioput != function->getInputs()->end(); ++ioput)
        // Add egde from message vertex to function vertex
        addEdge(generateStateGraphMessagesAddMessageToGraph(
                    (*ioput)->getMessageName()),
                task,
                (*ioput)->getMessageName(), Dependency::communication);
}

int XGraph::generateStateGraph(std::vector<XFunction*> functions,
        std::string startState, std::set<std::string> endStates) {
    std::vector<XFunction*>::iterator fit;

    // For each transition function
    for (fit = functions.begin(); fit != functions.end(); fit++) {
        // Add function as a task to the task list
        Task * functionTask = new Task((*fit)->getName(), Task::xfunction);
        // Add vertex for task
        addVertex(functionTask);
        // Add states
        generateStateGraphStates(*fit, functionTask, startState);
        // Add variable read writes
        generateStateGraphVariables(*fit, functionTask);
        // Add communication
        generateStateGraphMessages(*fit, functionTask);
        // If function next state is an end state
        if (endStates.find((*fit)->getNextState()) != endStates.end())
            // Add function task to end tasks list
            endTasks_.insert(functionTask);
    }

    return 0;
}

bool XGraph::containsVariableName(std::set<std::string> * variables,
        std::string variable) {
    std::set<std::string>::iterator it;
    for (it = variables->begin(); it != variables->end(); it++)
        if ((*it) == variable) return true;
    return false;
}

void XGraph::discoverLastVariableWrites(std::string variable,
        Vertex vertex,
        std::set<Vertex> * finished,
        std::set<Vertex> * writing) {
    boost::graph_traits<Graph>::in_edge_iterator iei, iei_end;

    // If vertex has already been seen then skip
    if (finished->find(vertex) == finished->end()) {
        for (boost::tie(iei, iei_end) = boost::in_edges(vertex, *graph_);
                            iei != iei_end; ++iei) {
            // Only if edge dependency is state or condition
            Dependency * d = getDependency((Edge)*iei);
            if (d->getDependencyType() == Dependency::state ||
                    d->getDependencyType() == Dependency::condition ||
                    d->getDependencyType() == Dependency::init) {
                // Get source vertex of the in edge
                Vertex v = boost::source((Edge)*iei, *graph_);

                if (containsVariableName(getTask(v)->getWriteVariables(),
                        variable))
                    // If vertex writes variable then
                    // add vertex to writing vertices
                    writing->insert(v);
                else
                    // If vertex does not write variable
                    // then try higher vertices
                    discoverLastVariableWrites(
                            variable, v, finished, writing);
            }
        }
        // Add vertex to finished list
        finished->insert(vertex);
    }
}

void XGraph::addEdgeToLastVariableWrites(std::set<std::string> rov,
        Vertex v) {
    // Iterators
    boost::graph_traits<Graph>::out_edge_iterator oei, oei_end;
    std::set<std::string>::iterator varit;
    std::set<Vertex>::iterator sit;
    // Vertex sets holding vertices already seen and writing vertices
    std::set<Vertex> finished;
    std::set<Vertex> writing;

    // For each reading variable
    for (varit = rov.begin(); varit != rov.end(); varit++) {
        // Discover vertices that write the variable last up the graph
        discoverLastVariableWrites(*varit, v, &finished, &writing);
        // For each writing vertex
        for (sit = writing.begin(); sit != writing.end(); ++sit) {
            // Find each out edge of the writing vertex
            boost::tie(oei, oei_end) = boost::out_edges(*sit, *graph_);
            for (; oei != oei_end; ++oei) {
                // Get the target vertex of the out edge
                Vertex v2 = boost::target((Edge)*oei, *graph_);
                // If the task is a variable and the same variable name
                if (getTask(v2)->getTaskType() == Task::xvariable &&
                        getTask(v2)->getName() == (*varit))
                    // Add an edge from the variable vertex
                    // to the reading vertex
                    addEdge(v2, v, (*varit), Dependency::variable);
            }
        }
    }
}

void removeVarWriteSet(std::string name,
        SetWritingTasks * lastWrites) {
    SetWritingTasks::iterator it;

    it = lastWrites->begin();
    while (it != lastWrites->end()) {
        SetWritingTasks::iterator current = it++;
        if ((*current).first == name)
            lastWrites->erase(current);
    }
}

void XGraph::addVariableVerticesToGraph(
        std::vector<XVariable*> * variables) {
    std::vector<Vertex> sorted_vertices;
    std::vector<Vertex>::iterator vit;
    std::set<std::string>::iterator varit;
    std::vector<XVariable*>::iterator i;
    std::pair<VertexIterator, VertexIterator> vp;
    std::set<Task*>::iterator t_it;

    // Add init function to provide first writes of all variables
    Task * initTask = new Task("Init", Task::init_agent);
    Vertex initVertex = addVertex(initTask);
    // Add edge from init to start vertex
    addEdge(initTask, startTask_, "Init", Dependency::init);
    // Add all variables to init task write list
    for (i = variables->begin(); i != variables->end(); i++) {
        initTask->addWriteVariable((*i)->getName());
        initTask->getLastWrites()->insert(std::make_pair((*i)->getName(), initVertex));
    }
    // Add end function to provide last writes to ioput
    endTask_ = new Task("End", Task::init_agent);
    addVertex(endTask_);
    for (t_it = endTasks_.begin(); t_it != endTasks_.end(); t_it++)
        addEdge((*t_it), endTask_, "End", Dependency::init);

    // Add edge from init to start vertex
    addEdge(initTask, startTask_, "Init", Dependency::init);
    // Add all variables to init task write list
    for (i = variables->begin(); i != variables->end(); i++) {
        initTask->addWriteVariable((*i)->getName());
        initTask->getLastWrites()->insert(std::make_pair((*i)->getName(), initVertex));
    }

//    std::cout << "initVertex = " << initVertex << std::endl;
//    std::cout << "startVertex_ = " << getVertex(startTask_) << std::endl;

//    std::cout << "1" << std::endl;

    // Create topological sorted list of vertices
    boost::topological_sort(*graph_, std::back_inserter(sorted_vertices)); // try front_inserter ?
    // The result is output in reverse topological order so reverse
    std::reverse(sorted_vertices.begin(), sorted_vertices.end());

//    std::cout << "2" << std::endl;

    // For each vertex in topological order
    for (vit = sorted_vertices.begin();
         vit != sorted_vertices.end(); vit++) {
//        std::cout << "2a" << std::endl;
        // If vertex is a function, condition, or init task
        Task * task = getTask((*vit));
//        std::cout << "2b" << std::endl;
        if (task->getTaskType() == Task::xfunction ||
                task->getTaskType() == Task::xcondition ||
                task->getTaskType() == Task::init_agent) {

//            std::cout << "Task: " << task->getName() << std::endl;

            // Copy last writing from incoming functions
            boost::graph_traits<Graph>::in_edge_iterator iei, iei_end;
            boost::tie(iei, iei_end) = boost::in_edges(*vit, *graph_);
            for (; iei != iei_end; ++iei) {
                Vertex v = boost::source((Edge)*iei, *graph_);
                Task * t = getTask(v);
                SetWritingTasks * lastWrites = t->getLastWrites();
                task->getLastWrites()->insert(lastWrites->begin(), lastWrites->end());
            }

            // For each read variable
            for (varit = task->getReadVariables()->begin();
                    varit != task->getReadVariables()->end(); varit++) {
                std::set<Vertex> alreadyUsed;
                SetWritingTasks::iterator wit;
                for (wit = task->getLastWrites()->begin();
                        wit != task->getLastWrites()->end(); wit++) {
                    // Check if variable edge has already been used
                    if (alreadyUsed.find(((*wit).second)) == alreadyUsed.end())
                        // If read variable equals last writes variable
                        if ((*wit).first == (*varit)) {
                            // Add edge
                            addEdge((Vertex)((*wit).second), (*vit),
                                    "", Dependency::variable);
                            // Add vertex to ones already used
                            alreadyUsed.insert(((*wit).second));
                        }
                }
            }

            // For writing variables create new vertex and add edge
            std::set<std::string> * rwv = task->getWriteVariables();
            for (varit = rwv->begin(); varit != rwv->end(); varit++) {
                // New vertex
            //    Vertex v = addVertex((*varit), Task::xvariable);
                // Edge to vertex
            //    addEdge((*vit), v, (*varit), Dependency::variable);
                // Remove writes from lastWrites
                removeVarWriteSet((*varit), task->getLastWrites());
                // Add new write
            //    task->getLastWrites()->insert(std::make_pair((*varit), v));
                task->getLastWrites()->insert(std::make_pair((*varit), (*vit)));
            }

            // Print out last writes
/*            SetWritingTasks::iterator swit;
            for (swit = task->getLastWrites()->begin();
                    swit != task->getLastWrites()->end(); swit++)
                std::cout << (*swit).first << " " << getTask((*swit).second)->getName() << std::endl;
*/
        }
    }
    // Remove init vertex
//    removeTask(initVertex);
}

void XGraph::addConditionDependencies() {
    std::set<size_t>::iterator it;
    std::vector<Vertex> sorted_vertices;
    std::vector<Vertex>::iterator vit;

    // Create topological sorted list of vertices
    boost::topological_sort(*graph_, std::back_inserter(sorted_vertices));
    // The result is output in reverse topological order so reverse
    std::reverse(sorted_vertices.begin(), sorted_vertices.end());

    // For each function/condition vertex
    for (vit = sorted_vertices.begin();
             vit != sorted_vertices.end(); vit++) {
        Task * task = getTask(*vit);
        if (task->getTaskType() == Task::xfunction ||
                task->getTaskType() == Task::xcondition) {
            // Copy last conditions from in edge tasks
            boost::graph_traits<Graph>::in_edge_iterator iei, iei_end;
            boost::tie(iei, iei_end) = boost::in_edges(*vit, *graph_);
            for (; iei != iei_end; ++iei) {
                Vertex v = boost::source((Edge)*iei, *graph_);
                Task * t = getTask(v);
                task->getLastConditions()->insert(t->getLastConditions()->begin(),
                        t->getLastConditions()->end());
            }
            // Add edge for each condition vertex found
            for (it = task->getLastConditions()->begin();
                    it != task->getLastConditions()->end(); it++)
                addEdge(*it, (*vit),
                        "Condition", Dependency::condition);
        }
        // If condition
        if (task->getTaskType() == Task::xcondition) {
            // Clear last conditions
            task->getLastConditions()->clear();
            // Add current condition
            task->getLastConditions()->insert(*vit);
        }
    }
}

bool compareTaskSets(std::set<size_t> a, std::set<size_t> b) {
    std::set<size_t>::iterator a_it, b_it;
    // Compare size first
    if (a.size() != b.size()) return false;
    // Compare elements
    for (a_it = a.begin(), b_it = b.begin();
            a_it != a.end(), b_it != b.end();
            a_it++, b_it++) {
        if ((*a_it) != (*b_it)) return false;
    }

    return true;
}

void XGraph::AddVariableOutput(std::vector<XVariable*> * variables) {
    // For each function that last writes a variable add dependency
    // to the data output of that variable
    std::pair<VertexIterator, VertexIterator> vp;
    std::vector<XVariable*>::iterator i;
    std::set<std::string>::iterator varit;
    std::vector<Vertex> vertexToDelete;
    std::vector<std::pair<std::string, std::set<size_t> > > varWrites;
    std::vector<std::pair<std::string, std::set<size_t> > >::iterator vwit;
    std::set<size_t>::iterator sit;

    SetWritingTasks * lws = endTask_->getLastWrites();
    SetWritingTasks::iterator wit;
    std::string varName = "";
    for (wit = lws->begin(); wit != lws->end(); wit++) {
        // If new var name
        if ((*wit).first != varName) {
            varName = (*wit).first;
            varWrites.insert(varWrites.begin(), make_pair((*wit).first, std::set<size_t>()));
        }
        (*varWrites.begin()).second.insert((*wit).second);
    }

/*    for (vwit = ++varWrites.begin(); vwit != varWrites.end(); vwit++) {
        std::cout << (*vwit).first << std::endl;
        for (sit = (*vwit).second.begin();
                sit != (*vwit).second.end(); sit++)
            std::cout << "\t" << (*sit) << " " << getTask((*sit))->getName() << std::endl;
    }
*/
    while (!varWrites.empty()) {
        // Create new io write task
        Task * task = new Task("", Task::io_pop_write);
        Vertex vertex = addVertex(task);
        task->getWriteVariables()->insert((*varWrites.begin()).first);
        // Check first var against other var task sets, if same then add to
        // current task and remove
        for (vwit = ++varWrites.begin(); vwit != varWrites.end();) {
            if (compareTaskSets((*varWrites.begin()).second, (*vwit).second)) {
                task->getWriteVariables()->insert((*vwit).first);
                vwit = varWrites.erase(vwit);
            } else {
                vwit++;
            }
        }
        // Add edges from each writing vector to task
        for (sit = (*varWrites.begin()).second.begin();
                sit != (*varWrites.begin()).second.end(); sit++)
            addEdge((*sit), vertex, "", Dependency::data);

        varWrites.erase(varWrites.begin());
    }

    // Remove end vertex
    removeTask(getVertex(endTask_));
}

void XGraph::removeStateDependencies() {
    EdgeIterator eit, eit_end;
    std::set<Edge> edgesToRemove;
    std::set<Edge>::iterator etrit;

    for (boost::tie(eit, eit_end) = boost::edges(*graph_);
            eit != eit_end; ++eit) {
        // Only if edge dependency is state or condition
        Dependency * d = getDependency((Edge)*eit);
        if (d->getDependencyType() == Dependency::state)
            edgesToRemove.insert((Edge)*eit);
    }
    for (etrit = edgesToRemove.begin(); etrit != edgesToRemove.end(); etrit++)
        removeDependency(*etrit);
}

void XGraph::addConditionVertices() {
    std::pair<VertexIterator, VertexIterator> vp;
    boost::graph_traits<Graph>::out_edge_iterator oei, oei_end;
    // For each vertex
    for (vp = boost::vertices(*graph_); vp.first != vp.second; ++vp.first) {
        // If out edges is larger than 1 and an xfunction task
        if (boost::out_degree(*vp.first, *graph_) > 1 &&
                getTask(*vp.first)->getTaskType() == Task::xstate) {
            getTask(*vp.first)->setTaskType(Task::xcondition);
            getTask(*vp.first)->setName("Condition");
        }
    }
}

void XGraph::setStartTask(Task * task) {
    startTask_ = task;
}

void XGraph::contractVertices(
        Task::TaskType taskType, Dependency::DependencyType dependencyType) {
    VertexIterator vi, vi_end;
    boost::graph_traits<Graph>::out_edge_iterator oei, oei_end;
    boost::graph_traits<Graph>::in_edge_iterator iei, iei_end;
    std::vector<Vertex> vertexToDelete;
    std::vector<Vertex>::iterator vit;
    // For each variable vertex
    for (boost::tie(vi, vi_end) = boost::vertices(*graph_);
            vi != vi_end; ++vi) {
        // If vertex is a variable
        if (getTask(*vi)->getTaskType() == taskType) {
            // Add an edge from all vertex sources to all vertex targets
            for (boost::tie(iei, iei_end) = boost::in_edges(*vi, *graph_);
                    iei != iei_end; ++iei)
                for (boost::tie(oei, oei_end) = boost::out_edges(*vi, *graph_);
                        oei != oei_end; ++oei)
                    addEdge(getTask(boost::source((Edge)*iei, *graph_)),
                            getTask(boost::target((Edge)*oei, *graph_)),
                            "", dependencyType);
            // Add vertex to delete list (as cannot delete vertex within
            // an iterator)
            vertexToDelete.push_back(*vi);
        }
    }
    // Delete vertices in delete list
    removeTasks(&vertexToDelete);
}

void XGraph::contractStateVertices() {
    // Change startVertex to the only and direct function under the current
    // state start vertex
    boost::graph_traits<Graph>::out_edge_iterator oei, oei_end;
    for (boost::tie(oei, oei_end) = boost::out_edges(getVertex(startTask_), *graph_);
                            oei != oei_end; ++oei)
        startTask_ = getTask(boost::target((Edge)*oei, *graph_));


    // Contract state tasks and replace with state dependency
    contractVertices(Task::xstate, Dependency::state);
}

void XGraph::contractVariableVertices() {
    // Contract variable tasks and replace with variable dependency
    contractVertices(Task::xvariable, Dependency::variable);
}

void XGraph::removeRedundantDependencies() {
    size_t ii;
    // The resultant transitive reduction graph
    Graph * trgraph = new Graph;
    std::vector<Task *> * trvertex2task =
            new std::vector<Task *>(vertex2task_->size());

    // Create a map to get a property of a graph, in this case the vertex index
    typedef boost::property_map<Graph, boost::vertex_index_t>::const_type
                VertexIndexMap;
    VertexIndexMap index_map = get(boost::vertex_index, *graph_);
    // A vector of vertices to hold trgraph vertices
    std::vector<Vertex> to_tc_vec(boost::num_vertices(*graph_));
    // Property map iterator
    // Iterator: Vertex *
    // OffsetMap: VertexIndexMap
    // Value type of iterator: Vertex
    // Reference type of iterator: Vertex&
    boost::iterator_property_map<Vertex *, VertexIndexMap, Vertex, Vertex&>
                    g_to_tc_map(&to_tc_vec[0], index_map);

    transitive_reduction(*graph_, *trgraph, g_to_tc_map, index_map);

    // Create new vertex task mapping for trgraph
    for (ii = 0; ii < boost::num_vertices(*graph_); ii++)
        trvertex2task->at(to_tc_vec[ii]) = vertex2task_->at(ii);

    // Make graph_ point to trgraph
    delete graph_;
    graph_ = trgraph;
    // Make vertex2task_ point to trvertex2task
    delete vertex2task_;
    vertex2task_ = trvertex2task;
    // Clear edge2dependency_ as edges no longer valid
    EdgeMap::iterator eit;
    for (eit = edge2dependency_->begin(); eit != edge2dependency_->end(); ++eit)
        delete ((*eit).second);
    edge2dependency_->clear();
}

/*!
 \brief Has cycle exception struct

 Has cycle exception used by cycle detector and passes
 back edge to already discovered vertex.
*/
struct has_cycle : public std::exception {
    explicit has_cycle(Edge d) : d_(d) {}
    const Edge edge() const throw() {
        return d_;
    }

  protected:
    Edge d_;
};

/*!
 \brief Visitor function cycle detector struct

 Visitor function object passed to depth_first_search.
 Contains back_edge method that is called when the depth_first_search
 explores an edge to an already discovered vertex.
*/
struct cycle_detector : public boost::default_dfs_visitor {
    void back_edge(Edge edge_t, const Graph &) {
        throw has_cycle(edge_t);
    }
};

int XGraph::checkCyclicDependencies() {
    // Visitor cycle detector for use with depth_first_search
    cycle_detector vis;

    try {
        // Depth first search applied to graph
        boost::depth_first_search(*graph_, visitor(vis));
    // If cyclic dependency is caught
    } catch(const has_cycle& err) {
        // Find associated dependency
        Dependency * d = getDependency(err.edge());
        // Find associated tasks
        Task * t1 = getTask(boost::source(err.edge(), *graph_));
        Task * t2 = getTask(boost::target(err.edge(), *graph_));
        std::fprintf(stderr,
                    "Error: cycle detected %s -> %s -> %s\n",
                    t1->getName().c_str(),
                    d->getName().c_str(),
                    t2->getName().c_str());
        return 1;
    }

    return 0;
}

int XGraph::checkFunctionConditions() {
    std::pair<VertexIterator, VertexIterator> vp;
    // For each vertex
    for (vp = boost::vertices(*graph_); vp.first != vp.second; ++vp.first) {
        // If state
        if (getTask(*vp.first)->getTaskType() == Task::xstate) {
            // If out edges is larger than 1
            if (boost::out_degree(*vp.first, *graph_) > 1) {
                boost::graph_traits<Graph>::out_edge_iterator oei, oei_end;
                // For each out edge
                for (boost::tie(oei, oei_end) =
                        boost::out_edges(*vp.first, *graph_);
                    oei != oei_end; ++oei) {
                    Task * t = getTask(boost::target((Edge)*oei, *graph_));
                    // If condition is null then return an error
                    if (!t->hasCondition()) {
                        std::fprintf(stderr,
            "Error: Function '%s' from a state with more than one %s\n",
                        t->getName().c_str(),
                        "out going function does not have a condition.");
                        return 1;
                    }
                }
            }
        }
    }

    return 0;
}

struct vertex_label_writer {
    explicit vertex_label_writer(std::vector<Task *> * vm) : vertex2task(vm) {}
    void operator()(std::ostream& out, const Vertex& v) const {
        Task * t = vertex2task->at(v);
        if (t->getTaskType() == Task::io_pop_write) {
            out << " [label=\"";
            std::set<std::string>::iterator it;
            for (it = t->getWriteVariables()->begin();
                    it != t->getWriteVariables()->end(); it++) {
                out << "" << (*it) << "\\n";
            }
            out << "\" shape=folder, style=filled, fillcolor=orange]";
        } else {
            out << " [label=\"" << t->getName() << "\"";
            if (t->getTaskType() == Task::xfunction)
                out << " shape=rect, style=filled, fillcolor=yellow";
            if (t->getTaskType() == Task::xcondition)
                out << " shape=invhouse, style=filled, fillcolor=yellow";
            if (t->getTaskType() == Task::init_agent)
                out << " shape=ellipse, style=filled, fillcolor=red";
            if (t->getTaskType() == Task::xvariable)
                out << " shape=ellipse";
            if (t->getTaskType() == Task::xmessage)
                out << " shape=parallelogram, style=filled, fillcolor=lightblue";
            if (t->getTaskType() == Task::io_pop_write)
                out << " shape=folder, style=filled, fillcolor=orange";
            out << "]";
        }
    }
  protected:
    std::vector<Task *> * vertex2task;
};

struct edge_label_writer {
    enum ArrowType { arrowForward = 0, arrowBackward };
    edge_label_writer(EdgeMap * em,
            ArrowType at) :
        edge2dependency(em),
          arrowType(at) {}
    void operator()(std::ostream& out, const Edge& e) const {
        Dependency * d = 0;
        EdgeMap::iterator it = edge2dependency->find(e);
        if (it != edge2dependency->end()) d = (*it).second;
        out << " [";
        if (d) out << "label=\"" << d->getGraphName() << "\" ";
        if (arrowType == edge_label_writer::arrowBackward) out << "dir = back";
        out << "]";
    }
  protected:
    EdgeMap * edge2dependency;
    ArrowType arrowType;
};

struct graph_writer {
    void operator()(std::ostream& out) const {
        // out << "node [shape = rect]" << std::endl;
    }
};

void XGraph::writeGraphviz(std::string fileName) {
    std::fstream graphfile;
    graphfile.open(fileName.c_str(), std::fstream::out);

    boost::write_graphviz(graphfile, *graph_,
            vertex_label_writer(vertex2task_),
            edge_label_writer(edge2dependency_,
                edge_label_writer::arrowForward),
            graph_writer());

    graphfile.clear();
}

#ifdef TESTBUILD
void XGraph::testBoostGraphLibrary() {
    std::vector<Vertex> breadth_first_vertices;
    std::vector<Vertex>::iterator vit;

    Vertex v0 = add_vertex(*graph_);

    add_edge(v0, 1, *graph_);
    add_edge(0, 2, *graph_);
    add_edge(0, 3, *graph_);
    add_edge(1, 7, *graph_);
    add_edge(2, 4, *graph_);
    add_edge(4, 7, *graph_);
    add_edge(2, 5, *graph_);
    add_edge(5, 6, *graph_);
    add_edge(6, 7, *graph_);
    add_edge(4, 5, *graph_);

    boost::topological_sort(*graph_,
            std::back_inserter(breadth_first_vertices));
    std::reverse(breadth_first_vertices.begin(), breadth_first_vertices.end());

    for (vit = breadth_first_vertices.begin();
             vit != breadth_first_vertices.end(); vit++) {
        std::cout << *vit << std::endl;
    }

    graph_->clear();
}
#endif

}}   // namespace flame::model
