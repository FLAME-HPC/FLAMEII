/*!
 * \file src/model/generate_task_list.cpp
 * \author Simon Coakley
 * \date 2012
 * \copyright Copyright (c) 2012 STFC Rutherford Appleton Laboratory
 * \copyright Copyright (c) 2012 University of Sheffield
 * \copyright GNU Lesser General Public License
 * \brief Generates task list
 */
#include <cstdio>
#include <string>
#include <vector>
#include <set>
#include <algorithm>
#include "./model_manager.hpp"
#include "./task.hpp"

namespace flame { namespace model {

int ModelManager::generateModelGraph(XModel * model, XGraph * modelGraph) {
    std::vector<XMachine*>::iterator agent;
    std::set<XGraph *> graphs;

    modelGraph->setAgentName(model->getName());

    // Consolidate agent graphs into a model graph
    for (agent = model->getAgents()->begin();
            agent != model->getAgents()->end(); agent++) {
        // Generate agent graph
        (*agent)->generateDependencyGraph();
        // Add to model graph
        //(*agent)->addToModelGraph(&modelGraph);
        graphs.insert((*agent)->getFunctionDependencyGraph());
    }

    modelGraph->importGraphs(graphs);

#ifdef TESTBUILD
    modelGraph->writeGraphviz(model->getName() + ".dot");
#endif

    return 0;
}

/*!
 * \brief Generates task list
 * \return The return code
 * Produces task list with state/data/communication dependencies.
 */
int ModelManager::registerModelWithTaskManager(XModel * model) {
//    int rc;

    XGraph modelGraph;

    generateModelGraph(model, &modelGraph);

    //modelGraph.generateTaskList(&tasks_);

    modelGraph.registerTasksAndDependenciesWithTaskManager();

    // printTaskList(&tasks_);

    return 0;
}

std::string ModelManager::taskTypeToString(Task::TaskType t) {
    std::string type;
    /* Convert Task task to printable string */
    if (t == Task::io_pop_write) type = "disk";
    else if (t == Task::sync_finish) type = "syncf";
    else if (t == Task::sync_start) type = "syncs";
    else if (t == Task::xfunction) type = "func";
    else if (t == Task::xcondition) type = "cond";
    else if (t == Task::start_model) type = "startm";
    else if (t == Task::start_agent) type = "starta";
    /* If task not recognised return empty string */
    else
        type = "";
    return type;
}

void ModelManager::printTaskList(std::vector<Task*> * tasks) {
    std::vector<Task*>::iterator task;
    std::set<std::string>::iterator it;

    // Print column headers
    fprintf(stdout, "Level\tPriority\tType\tName\n");
    fprintf(stdout, "-----\t--------\t----\t----\n");
    // For eac task
    for (task = tasks->begin(); task != tasks->end(); ++task) {
        std::string name;
        // If disk output then print all variables
        if ((*task)->getTaskType() == Task::io_pop_write) {
            for (it = (*task)->getWriteVariables()->begin();
                    it != (*task)->getWriteVariables()->end(); it++) {
                name.append((*it) + " ");
            }
        // Else just print the task name
        } else {
            name = (*task)->getName().c_str();
        }
        // Print level, priority, type, and name
        fprintf(stdout, "%u\t%u\t\t%s\t%s\n",
                static_cast<unsigned int>((*task)->getLevel()),
                static_cast<unsigned int>((*task)->getPriorityLevel()),
                taskTypeToString((*task)->getTaskType()).c_str(),
                name.c_str());
    }
}

int ModelManager::calculate_dependencies(std::vector<Task*> * tasks) {
    std::vector<Task*>::iterator task;
//    size_t ii;

    /* Initialise task levels to be zero */
    for (task = tasks->begin(); task != tasks->end(); ++task)
        (*task)->setLevel(0);

    /* Calculate layers of dependency graph */
    /* This is achieved by finding functions with no dependencies */
    /* giving them a layer no, taking those functions away and doing
     * the operation again. */
    /* Boolean to track if all tasks have been leveled */
    bool finished = false;
    /* The current level that is being populated */
    size_t currentLevel = 1;
    /* Loop while tasks still being leveled */
    while (!finished) {
        finished = true;    /* Set finished to be true, until unleveled task */
        /* For every task */
        for (task = tasks->begin(); task != tasks->end(); ++task) {
            /* If task is not leveled */
            if ((*task)->getLevel() == 0) {
                /* Boolean to track if any dependencies
                 * still need to be leveled */
                bool unleveled_dependency = false;
                /* For each dependency */
                /* Didn't use iterator here as caused valgrind errors */
/*                for (ii = 0; ii < (*task)->getParents().size(); ii++) {
                    Dependency * dependency = (*task)->getParents().at(ii);
                    // If the dependency is not leveled or just been leveled
                    // at the current level that is being populated
                    if ((dependency)->getTask()->getLevel() == 0 ||
                        (dependency)->getTask()->getLevel() == currentLevel)
                        // Set that current task has an unleveled dependency
                        unleveled_dependency = true;
                }*/
                /* If no unleveled dependencies */
                if (!unleveled_dependency)
                    /* Add task to current level */
                    (*task)->setLevel(currentLevel);
                else
                    /* Else leveling has not finished */
                    finished = false;
            }
        }
        /* Increment current level */
        currentLevel++;
    }

    return 0;
}

bool compare_task_levels(Task * i, Task * j) {
    /* If same level then use priority level */
    if (i->getLevel() == j->getLevel()) {
        return (i->getPriorityLevel() > j->getPriorityLevel());
    } else {
        return (i->getLevel() < j->getLevel());
    }
}

int ModelManager::calculate_task_list(std::vector<Task*> * tasks) {
    /* Sort the task list by level */
    sort(tasks->begin(), tasks->end(), compare_task_levels);

    printTaskList(tasks);

    return 0;
}

}}  // namespace flame::model
