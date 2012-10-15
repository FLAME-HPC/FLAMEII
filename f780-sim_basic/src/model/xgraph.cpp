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
#include <boost/lexical_cast.hpp>
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
#include "exe/task_manager.hpp"
#include "include/flame.h"

namespace flame { namespace model {

XGraph::XGraph() {
    // Initialise pointers
    graph_ = new Graph;
    vertex2task_ = new std::vector<Task *>;
    edge2dependency_ = new EdgeMap;
    taskImported_ = false;
    endTask_ = 0;
    startTask_ = 0;
}

XGraph::~XGraph() {
    std::vector<Task *>::iterator vit;
    /* Free task memory */
    if (!taskImported_)
        for (vit = vertex2task_->begin(); vit != vertex2task_->end(); vit++)
            delete (*vit);
    // Free vertex task mapping
    delete vertex2task_;
    // Free edge dependency mapping
    delete edge2dependency_;
    // Free graph
    delete graph_;
}

std::vector<Task *> * XGraph::getVertexTaskMap() {
    return vertex2task_;
}

void XGraph::setTasksImported(bool b) {
    taskImported_ = b;
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

void XGraph::removeVertex(Vertex v) {
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
void XGraph::removeVertices(std::vector<Vertex> * tasks) {
    std::vector<Vertex>::iterator vit;
    // Sort vertices largest first
    std::sort(tasks->begin(), tasks->end(), std::greater<size_t>());
    // Remove tasks in order
    for (vit = tasks->begin(); vit != tasks->end(); ++vit)
        removeVertex((*vit));
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

Edge XGraph::addEdge(Vertex to, Vertex from, std::string name,
        Dependency::DependencyType type) {
    // Create dependency from name and type
    Dependency * d = new Dependency(name, type);
    // Add edge to graph
    std::pair<Edge, bool> e = add_edge(to, from, *graph_);
    // Add mapping from edge to dependency
    edge2dependency_->insert(std::make_pair(e.first, d));
    // Return edge
    return e.first;
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
    printf("a1\n");
#ifdef TESTBUILD
    writeGraphviz(agentName_ + "_1.dot");
#endif
    // Transform conditional states (more than one out edge)
    // into a condition task
    transformConditionalStatesToConditions();
    // Contract state vertices
    contractStateVertices();
    printf("a2\n");
#ifdef TESTBUILD
    writeGraphviz(agentName_ + "_2.dot");
#endif
    printf("a3\n");
    // Add data and condition dependencies
    addDataAndConditionDependencies(variables);
    // Remove state dependencies
    removeStateDependencies();
#ifdef TESTBUILD
    writeGraphviz(agentName_ + "_3.dot");
#endif
    // Add data output tasks
    AddVariableOutput(variables);
#ifdef USE_VARIABLE_VERTICES
    // Contract variable vertices
    contractVariableVertices();
#endif

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
    Task * task = new Task(agentName_, name, Task::xstate);
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
    addEdge(getVertex(currentState), getVertex(task),
            function->getCurrentState(), Dependency::state);
    // Add edge from function to next state
    addEdge(getVertex(task), getVertex(nextState),
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
        for (sit = rov->begin(); sit != rov->end(); sit++) {
            currentState->addReadVariable(*sit);
            currentState->addReadOnlyVariable(*sit);
        }
    }
}

void XGraph::generateStateGraphVariables(XFunction * function, Task * task) {
    std::vector<std::string>::iterator sitv;
    // For each read only variable
    for (sitv = function->getReadOnlyVariables()->begin();
            sitv != function->getReadOnlyVariables()->end(); sitv++) {
        // Add to task read only variables
        task->addReadOnlyVariable(*sitv);
        // Add to task read variables
        task->addReadVariable(*sitv);
    }
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
    Task * task = new Task(name, name, Task::xmessage);
    addVertex(task);
    return task;
}

void XGraph::generateStateGraphMessages(XFunction * function, Task * task) {
    std::vector<XIOput*>::iterator ioput;
    // Find outputting functions
    for (ioput = function->getOutputs()->begin();
         ioput != function->getOutputs()->end(); ++ioput) {
        // Add output message to function task
        task->addOutputMessage((*ioput)->getMessageName());
        // Add edge from function vertex to message vertex
        addEdge(getVertex(task),
                getVertex(generateStateGraphMessagesAddMessageToGraph(
                    (*ioput)->getMessageName())),
                (*ioput)->getMessageName(), Dependency::communication);
    }
    // Find inputting functions
    for (ioput = function->getInputs()->begin();
         ioput != function->getInputs()->end(); ++ioput) {
        // Add input message to function task
        task->addInputMessage((*ioput)->getMessageName());
        // Add egde from message vertex to function vertex
        addEdge(getVertex(generateStateGraphMessagesAddMessageToGraph(
                    (*ioput)->getMessageName())),
                    getVertex(task),
                (*ioput)->getMessageName(), Dependency::communication);
    }
}

int XGraph::generateStateGraph(std::vector<XFunction*> functions,
        std::string startState, std::set<std::string> endStates) {
    std::vector<XFunction*>::iterator fit;

    // For each transition function
    for (fit = functions.begin(); fit != functions.end(); fit++) {
        // Add function as a task to the task list
        Task * functionTask = new Task(agentName_,
                (*fit)->getName(), Task::xfunction);
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

void XGraph::generateTaskList(std::vector<Task*> * tasks) {
    std::vector<Vertex> sorted_vertices;
    std::vector<Vertex>::reverse_iterator vit;

    // Create topological sorted list of vertices
    boost::topological_sort(*graph_, std::back_inserter(sorted_vertices));

    for (vit = sorted_vertices.rbegin();
            vit != sorted_vertices.rend(); vit++)
        tasks->push_back(getTask((*vit)));
}

int XGraph::registerAllowAccess(flame::exe::Task * task,
        std::set<std::string> * vars, bool writeable) {
    std::set<std::string>::iterator sit;

    // For each variable name
    for (sit = vars->begin();
            sit != vars->end(); sit++) {
        try {
            // Allow access for variable name
            task->AllowAccess((*sit), writeable);
        }
        catch(const flame::exceptions::logic_error& E) {
            std::fprintf(stderr, "Error: %s\n", E.what());
            std::fprintf(stderr, "When allowing access for '%s' variable\n",
                    (*sit).c_str());
            return 1;
        }
    }

    return 0;
}

int XGraph::registerAllowMessage(flame::exe::Task * task,
        std::set<std::string> * messages, bool post) {
    std::set<std::string>::iterator sit;

    // For each message
    for (sit = messages->begin();
            sit != messages->end(); sit++) {
        // Update task with appropriate access
        try {
            // If output message then allow post
            if (post) task->AllowMessagePost((*sit));
            // If input message then allow read
            else
                task->AllowMessageRead((*sit));
        }
        catch(const flame::exceptions::logic_error& E) {
            std::fprintf(stderr, "Error: %s\n", E.what());
            std::fprintf(stderr, "When allowing access for '%s' message\n",
                    (*sit).c_str());
            return 1;
        }
    }

    return 0;
}

int XGraph::registerAgentTask(Task * t,
        std::map<std::string, flame::exe::TaskFunction> funcMap) {
    flame::exe::TaskManager& taskManager = exe::TaskManager::GetInstance();
    std::map<std::string, flame::exe::TaskFunction>::iterator it;
    int rc = 0;

    // Try and find function pointer from map
    it = funcMap.find(t->getName());
    if (it == funcMap.end()) {
        std::fprintf(stderr, "Error: function '%s' has not be registered %s\n",
        t->getTaskName().c_str(), "and therefore a task cannot be created");
        return 3;
    }

    try {
        // Create agent task
        flame::exe::Task& task = taskManager.CreateAgentTask(
                t->getTaskName(), t->getParentName(), ((*it).second));
        // Allow access to read only variables
        rc += registerAllowAccess(&task, t->getReadOnlyVariables(), false);
        // Allow access to read write variables
        rc += registerAllowAccess(&task, t->getWriteVariables(), true);
        // Add access to output messages
        rc += registerAllowMessage(&task, t->getOutputMessages(), true);
        // Add access to input messages
        rc += registerAllowMessage(&task, t->getInputMessages(), false);
        if (rc != 0) return 1;
    }
    catch(const flame::exceptions::flame_exception& E) {
    std::fprintf(stderr, "Error: %s\nWhen creating a task for '%s' function\n",
            E.what(), t->getTaskName().c_str());
        return 2;
    }

    return 0;
}

int XGraph::registerDataTask(Task * t) {
    // std::string taskName = t->getTaskName();
    std::string agentName = t->getParentName();

    try {
    // printf("Adding task %s io task\n", taskName.c_str());
    // Hard coded agent name as model tasks don't have an agent!
    // taskManager.CreateAgentTask(taskName, "Circle", ((*it).second));
    }
    catch(const flame::exceptions::flame_exception& E) {
std::fprintf(stderr, "Error: %s\nWhen creating an io task for '%s' agent\n",
                E.what(),
                agentName.c_str());
        return 2;
    }

    return 0;
}

int XGraph::registerMessageTask(Task * t) {
    flame::exe::TaskManager& taskManager = exe::TaskManager::GetInstance();

    try {
        // If sync task then create sync task
        if (t->getTaskType() == Task::xmessage_sync)
            taskManager.CreateMessageBoardTask(t->getTaskName(), t->getName(),
                exe::MessageBoardTask::OP_SYNC);
        // If clear task then create clear task
        if (t->getTaskType() == Task::xmessage_clear)
            taskManager.CreateMessageBoardTask(t->getTaskName(), t->getName(),
                exe::MessageBoardTask::OP_CLEAR);
    }
    catch(const flame::exceptions::logic_error& E) {
        std::fprintf(stderr,
            "Error: %s\nWhen creating a sync task for '%s' message\n",
            E.what(), t->getName().c_str());
        return 2;
    }

    return 0;
}

int XGraph::registerDependencies() {
    flame::exe::TaskManager& taskManager = exe::TaskManager::GetInstance();

    boost::graph_traits<Graph>::edge_iterator iei, iei_end;
    // Add all in and out vertex edges to set of edges to be removed
    for (boost::tie(iei, iei_end) = boost::edges(*graph_);
            iei != iei_end; ++iei) {
        std::string source = getTask(
                boost::source((Edge)*iei, *graph_))->getTaskName();
        std::string target = getTask(
                boost::target((Edge)*iei, *graph_))->getTaskName();
        try { taskManager.AddDependency(target, source); }
        catch(const flame::exceptions::flame_exception& E) {
            std::fprintf(
            stderr, "Error: %s\nWhen adding a dependency between %s and %s\n",
            E.what(), source.c_str(), target.c_str());
        }
    }

    return 0;
}

int XGraph::registerTasksAndDependenciesWithTaskManager(
        std::map<std::string, flame::exe::TaskFunction> funcMap) {
    flame::exe::TaskManager& taskManager = exe::TaskManager::GetInstance();
    std::pair<VertexIterator, VertexIterator> vp;

    // For each vertex
    for (vp = boost::vertices(*graph_); vp.first != vp.second; ++vp.first) {
        Task * t = getTask(*vp.first);
        Task::TaskType type = t->getTaskType();

        // If agent task
        if (type == Task::xfunction || type == Task::xcondition)
            registerAgentTask(t, funcMap);
        // If data task
        if (type == Task::start_model || type == Task::finish_model ||
                type == Task::io_pop_write)
            registerDataTask(t);
        // If message task
        if (type == Task::xmessage_sync || type == Task::xmessage_clear)
            registerMessageTask(t);
    }

    // Register dependencies with the Task Manager
    registerDependencies();

    // Once finalised, tasks and dependencies can no longer be added
    taskManager.Finalise();

    return 0;
}

void clearVarWriteSet(std::string name,
        VarMapToVertices * lastWrites) {
    VarMapToVertices::iterator it = lastWrites->find(name);
    if (it != lastWrites->end()) (*it).second.clear();
}

std::set<size_t> * getVertexSet(
        std::string name, VarMapToVertices * lastWrites) {
    VarMapToVertices::iterator it = lastWrites->find(name);
    if (it != lastWrites->end()) return &(*it).second;
    // If name not found then add
    return &(*(lastWrites->insert(
            std::make_pair(name, std::set<size_t>())).first)).second;
}

void addVectorToVarWriteSet(std::string name, Vertex v,
        VarMapToVertices * lastWrites) {
    getVertexSet(name, lastWrites)->insert(v);
}

void copyVarWriteSets(VarMapToVertices * from, VarMapToVertices * to) {
    VarMapToVertices::iterator it;
    std::set<size_t> * vset;

    // For each var in from
    for (it = from->begin(); it != from->end(); it++) {
        // Get associated var vertex set from to
        vset = getVertexSet((*it).first, to);
        // Insert from set to to set
        vset->insert((*it).second.begin(), (*it).second.end());
    }
}

void XGraph::addStartTask(std::vector<XVariable*> * variables) {
    std::vector<XVariable*>::iterator i;
    // Add init function to provide first writes of all variables
    Task * initTask = new Task(agentName_, std::string(agentName_),
            Task::start_agent);
    Vertex initVertex = addVertex(initTask);
    // Add edge from init to start vertex
    addEdge(getVertex(initTask), getVertex(startTask_),
            "Start", Dependency::init);
    // Add all variables to init task write list
    for (i = variables->begin(); i != variables->end(); i++) {
        initTask->addWriteVariable((*i)->getName());
        addVectorToVarWriteSet((*i)->getName(), initVertex,
                initTask->getLastWrites());
    }
}

void XGraph::addEndTask() {
    std::set<Task*>::iterator t_it;
    // Add end function to provide last writes to ioput
    endTask_ = new Task(agentName_, std::string(agentName_),
            Task::finish_agent);
    Vertex v = addVertex(endTask_);
    for (t_it = endTasks_.begin(); t_it != endTasks_.end(); t_it++)
        addEdge(getVertex((*t_it)), v, "End", Dependency::init);
}

void XGraph::copyWritingAndConditionVerticesFromInEdges(Vertex v, Task * task) {
    boost::graph_traits<Graph>::in_edge_iterator iei, iei_end;

    // For each in edge task
    boost::tie(iei, iei_end) = boost::in_edges(v, *graph_);
    for (; iei != iei_end; ++iei) {
        Task * t = getTask(boost::source((Edge)*iei, *graph_));
        // Copy last writing from incoming functions
        copyVarWriteSets(t->getLastWrites(), task->getLastWrites());
        // Copy last conditions from in edge tasks
        task->getLastConditions()->insert(
                t->getLastConditions()->begin(),
                t->getLastConditions()->end());
    }
}

void XGraph::addConditionDependenciesAndUpdateLastConditions(
        Vertex v, Task * t) {
    std::set<size_t>::iterator it;

    // Add edge for each condition vertex found
    for (it = t->getLastConditions()->begin();
            it != t->getLastConditions()->end(); it++)
        addEdge(*it, v, "Condition", Dependency::condition);
    // If condition
    if (t->getTaskType() == Task::xcondition) {
        // Clear last conditions
        t->getLastConditions()->clear();
        // Add current condition
        t->getLastConditions()->insert(v);
    }
}

void XGraph::addReadDependencies(Vertex v, Task * t) {
    std::set<std::string>::iterator varit;
    VarMapToVertices::iterator wit;
    std::set<size_t>::iterator it;
    // For each read variable
    std::set<Vertex> alreadyUsed;
    for (varit = t->getReadVariables()->begin();
            varit != t->getReadVariables()->end(); varit++) {
        for (wit = t->getLastWrites()->begin();
                wit != t->getLastWrites()->end(); wit++) {
            // If read variable equals last writes variable
            if ((*wit).first == (*varit)) {
                // For each task
                for (it = (*wit).second.begin();
                        it != (*wit).second.end(); it++) {
                    if (alreadyUsed.find(*it) == alreadyUsed.end()) {
                        // Add edge
#ifdef USE_VARIABLE_VERTICES
                        addEdge(*it, v, *varit, Dependency::variable);
#else
                        addEdge(*it, v, "Data", Dependency::variable);
                        // Add vertex to ones already used
                        alreadyUsed.insert(*it);
#endif
                    }
                }
            }
        }
    }
}

void XGraph::addWritingVerticesToList(Vertex v, Task * t) {
    std::set<std::string>::iterator varit;
    // For writing variables create new vertex and add edge
    std::set<std::string> * rwv = t->getWriteVariables();
    for (varit = rwv->begin(); varit != rwv->end(); varit++) {
        // Remove writes from lastWrites
        clearVarWriteSet((*varit), t->getLastWrites());
#ifdef USE_VARIABLE_VERTICES
        // New vertex
        Task * task = new Task(agentName_, (*varit), Task::xvariable);
        Vertex varVertex = addVertex(task);
        // Edge to vertex
        addEdge(v, varVertex, (*varit), Dependency::variable);
        // Add new write
        addVectorToVarWriteSet((*varit), varVertex, t->getLastWrites());
#else
        // Add new write
        addVectorToVarWriteSet((*varit), v, t->getLastWrites());
#endif
    }
}

void XGraph::addDataAndConditionDependencies(
        std::vector<XVariable*> * variables) {
    std::vector<Vertex>::reverse_iterator vit;
    std::vector<Vertex> sorted_vertices;

    // Add start and end tasks
    addStartTask(variables);
    addEndTask();

    // Create topological sorted list of vertices
    boost::topological_sort(*graph_, std::back_inserter(sorted_vertices));

    // For each vertex in topological order
    // Iterate in reverse as output order is reversed
    for (vit = sorted_vertices.rbegin(); vit != sorted_vertices.rend(); vit++) {
        // If vertex is a function, condition, or init task
        Task * task = getTask((*vit));
        if (task->getTaskType() == Task::xfunction ||
                task->getTaskType() == Task::xcondition ||
                task->getTaskType() == Task::start_agent ||
                task->getTaskType() == Task::finish_agent) {
            // Copy variable writing vertices and condition vertices
            // from in coming vertices
            copyWritingAndConditionVerticesFromInEdges(*vit, task);
            // Add dependencies on condition list and update if a condition
            addConditionDependenciesAndUpdateLastConditions(*vit, task);
            // Add dependencies on writing vertices that this vertex reads
            addReadDependencies(*vit, task);
            // Add vertices to variable writing vertices list
            // for writing variables
            addWritingVerticesToList(*vit, task);
        }
    }
}

bool XGraph::compareTaskSets(std::set<size_t> a, std::set<size_t> b) {
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
    VarMapToVertices::iterator vwit;
    std::set<size_t>::iterator sit;
    VarMapToVertices * lws = endTask_->getLastWrites();
    size_t count;

    while (!lws->empty()) {
        // Create new io write task
        Task * task = new Task(agentName_,
            boost::lexical_cast<std::string>(count++), Task::io_pop_write);
        Vertex vertex = addVertex(task);
        task->getWriteVariables()->insert((*lws->begin()).first);
        // Check first var against other var task sets, if same then add to
        // current task and remove
        for (vwit = ++lws->begin(); vwit != lws->end();) {
            if (compareTaskSets((*lws->begin()).second, (*vwit).second)) {
                task->getWriteVariables()->insert((*vwit).first);
                lws->erase(vwit++);
            } else {
                vwit++;
            }
        }
        // Add edges from each writing vector to task
        for (sit = (*lws->begin()).second.begin();
                sit != (*lws->begin()).second.end(); sit++)
            addEdge((*sit), vertex, "", Dependency::data);

        lws->erase(lws->begin());
    }

    // Remove end vertex
    removeVertex(getVertex(endTask_));
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

void XGraph::transformConditionalStatesToConditions() {
    std::pair<VertexIterator, VertexIterator> vp;
    boost::graph_traits<Graph>::out_edge_iterator oei, oei_end;
    size_t count;

    // For each vertex
    for (vp = boost::vertices(*graph_); vp.first != vp.second; ++vp.first) {
        // If out edges is larger than 1 and a state task
        if (boost::out_degree(*vp.first, *graph_) > 1 &&
                getTask(*vp.first)->getTaskType() == Task::xstate) {
            // Change task type to a condition
            getTask(*vp.first)->setTaskType(Task::xcondition);
            getTask(*vp.first)->setName(
                    boost::lexical_cast<std::string>(count++));
            getTask(*vp.first)->setPriorityLevel(5);
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
                    addEdge(boost::source((Edge)*iei, *graph_),
                            boost::target((Edge)*oei, *graph_),
                            "", dependencyType);
            // Add vertex to delete list (as cannot delete vertex within
            // an iterator)
            vertexToDelete.push_back(*vi);
        }
    }
    // Delete vertices in delete list
    removeVertices(&vertexToDelete);
}

void XGraph::contractStateVertices() {
    // Change startVertex to the only and direct function under the current
    // state start vertex
    boost::graph_traits<Graph>::out_edge_iterator oei, oei_end;
    // If startTask a state (not a state converted to a condition
    if (startTask_->getTaskType() == Task::xstate)
        for (boost::tie(oei, oei_end) =
                boost::out_edges(getVertex(startTask_), *graph_);
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

Vertex XGraph::getMessageVertex(std::string name, Task::TaskType type) {
    size_t ii;
    // For each task
    for (ii = 0; ii < vertex2task_->size(); ii++)
        // If find message and type then return
        if (vertex2task_->at(ii)->getName() == name &&
                vertex2task_->at(ii)->getTaskType() == type)
            return ii;
    // Otherwise create new vertex and return
    Task * t = new Task(name, name, type);
    Vertex v = addVertex(t);
    return v;
}

void XGraph::changeMessageTasksToSync() {
    boost::graph_traits<Graph>::out_edge_iterator oei, oei_end;
    boost::graph_traits<Graph>::in_edge_iterator iei, iei_end;
    std::vector<Vertex> vertexToDelete;
    std::set<Edge> edgesToRemove;
    std::set<Edge>::iterator etrit;
    size_t ii;
    // For each task
    for (ii = 0; ii < vertex2task_->size(); ii++) {
        Task * t = vertex2task_->at(ii);
        // If message task
        if (t->getTaskType() == Task::xmessage) {
            // Create start and finish syncs
            Vertex s = getMessageVertex(t->getName(), Task::xmessage_sync);
            // For each incoming edge to message add to start
            for (boost::tie(iei, iei_end) = boost::in_edges(ii, *graph_);
                            iei != iei_end; ++iei) {
                add_edge(boost::source(*iei, *graph_), s, *graph_);
                edgesToRemove.insert(*iei);
            }
            // For each out going edge to message add to finish
            for (boost::tie(oei, oei_end) = boost::out_edges(ii, *graph_);
                            oei != oei_end; ++oei) {
                add_edge(s, boost::target(*oei, *graph_), *graph_);
                edgesToRemove.insert(*oei);
            }
            // delete message task
            vertexToDelete.push_back(ii);
        }
    }
    // Delete edges
    for (etrit = edgesToRemove.begin(); etrit != edgesToRemove.end(); etrit++)
        removeDependency(*etrit);
    // Delete vertices in delete list
    removeVertices(&vertexToDelete);
}

void XGraph::import(XGraph * graph) {
    std::vector<Task *> * v2t = graph->getVertexTaskMap();
    size_t ii;
    std::map<Vertex, Vertex> import2new;
    EdgeIterator eit, end;

    // For each task vertex map
    for (ii = 0; ii < v2t->size(); ii++) {
        // Add vertex to current graph
        Vertex v = addVertex(v2t->at(ii));
        // Add to vertex to vertex map
        import2new.insert(std::make_pair(ii, v));
        // If task is an init agent then add edge
        if (v2t->at(ii)->getTaskType() == Task::start_agent)
        add_edge(getVertex(startTask_), v, *graph_);
        // If task is a data output task then add edge
        if (v2t->at(ii)->getTaskType() == Task::io_pop_write)
        add_edge(v, getVertex(endTask_), *graph_);
    }
    // For each edge
    for (boost::tie(eit, end) = boost::edges(*(graph->getGraph()));
            eit != end; eit++) {
        // Add edge using vertex to vertex map
        Vertex s = boost::source(*eit, *(graph->getGraph()));
        Vertex t = boost::target(*eit, *(graph->getGraph()));
        Vertex ns = (*(import2new.find(s))).second;
        Vertex nt = (*(import2new.find(t))).second;
        add_edge(ns, nt, *graph_);
    }

    graph->setTasksImported(true);
}

void XGraph::importGraphs(std::set<XGraph*> graphs) {
    std::set<XGraph*>::iterator it;

    // Add start task
    Task * t = new Task(agentName_, "Start", Task::start_model);
    addVertex(t);
    startTask_ = t;
    // Add finish task
    t = new Task(agentName_, "Finish", Task::finish_model);
    addVertex(t);
    endTask_ = t;

    for (it = graphs.begin(); it != graphs.end(); it++)
        import((*it));

    // Contract start agents
    contractVertices(Task::start_agent, Dependency::blank);
    // Add message sync tasks
    changeMessageTasksToSync();
    // Add message clear tasks
    addMessageClearTasks();
}

void XGraph::addMessageClearTasks() {
    VertexIterator vi, vi_end;
    boost::graph_traits<Graph>::out_edge_iterator oei, oei_end;

    // For each variable vertex
    for (boost::tie(vi, vi_end) = boost::vertices(*graph_);
            vi != vi_end; ++vi) {
        Task * t = getTask((*vi));
        // For each message sync task
        if (t->getTaskType() == Task::xmessage_sync) {
            // Create message clear task
            Task * task = new Task(t->getParentName(),
                    t->getName(), Task::xmessage_clear);
            Vertex clearV = addVertex(task);
            // Get target tasks
            for (boost::tie(oei, oei_end) =
                boost::out_edges((*vi), *graph_); oei != oei_end; ++oei) {
                // Add edge from target tasks to clear task
                add_edge(boost::target((Edge)*oei, *graph_), clearV, *graph_);
            }
        }
    }
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
            out << " [label=\"";
            if (t->getTaskType() == Task::xmessage_sync)
                out << "SYNC: " << t->getName() << "\"";
            else if (t->getTaskType() == Task::xmessage_clear)
                out << "CLEAR: " << t->getName() << "\"";
            else if (t->getTaskType() == Task::start_agent ||
                    t->getTaskType() == Task::start_model)
                out << "Start\\n" << t->getParentName() << "\"";
            else if (t->getTaskType() == Task::finish_agent ||
                    t->getTaskType() == Task::finish_model)
                out << "Finish\\n" << t->getParentName() << "\"";
            else
                out << t->getName() << "\"";
            if (t->getTaskType() == Task::xfunction)
                out << " shape=rect, style=filled, fillcolor=yellow";
            if (t->getTaskType() == Task::xcondition)
                out << " shape=invhouse, style=filled, fillcolor=yellow";
            if (t->getTaskType() == Task::start_agent ||
                    t->getTaskType() == Task::finish_agent ||
                    t->getTaskType() == Task::start_model ||
                    t->getTaskType() == Task::finish_model)
                out << " shape=ellipse, style=filled, fillcolor=red";
            if (t->getTaskType() == Task::xvariable ||
                    t->getTaskType() == Task::xstate)
                out << " shape=ellipse, style=filled, fillcolor=white";
            if (t->getTaskType() == Task::xmessage_clear ||
                    t->getTaskType() == Task::xmessage_sync) {
                out << " shape=parallelogram, style=filled, ";
                out << "fillcolor=lightblue";
            }
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
        if (d) if (d->getDependencyType() != Dependency::blank)
            out << "label=\"" << d->getGraphName() << "\" ";
        if (arrowType == edge_label_writer::arrowBackward) out << "dir = back";
        out << "]";
    }
  protected:
    EdgeMap * edge2dependency;
    ArrowType arrowType;
};

struct graph_writer {
    void operator()(std::ostream& /*out*/) const {
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
/*    std::vector<Vertex> breadth_first_vertices;
    std::vector<Vertex>::iterator vit;

    Vertex s = add_vertex(*graph_);

    add_edge(s, 1, *graph_);
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

    graph_->clear();*/
}

void printLastWrites(VarMapToVertices * lws) {
    VarMapToVertices::iterator vwit;
    std::set<size_t>::iterator sit;
    // Print last writes
    for (vwit = lws->begin(); vwit != lws->end(); vwit++) {
        std::cout << (*vwit).first << std::endl;
        for (sit = (*vwit).second.begin(); sit != (*vwit).second.end(); sit++)
            std::cout << "\t" << (*sit) << std::endl;
    }
}

bool XGraph::testCompareTaskSets() {
    VarMapToVertices a;

    addVectorToVarWriteSet("b", 1, &a);
    addVectorToVarWriteSet("a", 0, &a);
    addVectorToVarWriteSet("b", 2, &a);
    addVectorToVarWriteSet("a", 1, &a);

    clearVarWriteSet("a", &a);

    // printLastWrites(&a);

    return true;
}
#endif

}}   // namespace flame::model
