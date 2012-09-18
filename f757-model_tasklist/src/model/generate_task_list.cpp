/*!
 * \file src/model/generate_task_list.cpp
 * \author Simon Coakley
 * \date 2012
 * \copyright Copyright (c) 2012 STFC Rutherford Appleton Laboratory
 * \copyright Copyright (c) 2012 University of Sheffield
 * \copyright GNU Lesser General Public License
 * \brief Generates task list
 */
#include <string>
#include <vector>
#include <algorithm>
#include "./model_manager.hpp"
#include "./task.hpp"

namespace flame { namespace model {

/*!
 * \brief Generates task list
 * \return The return code
 * Produces task list with state/data/communication dependencies.
 */
int ModelManager::generate_task_list() {
//    int rc;
    std::vector<XMachine*>::iterator agent;

    // Calculate dependencies
//    rc = calculate_dependencies(&tasks_);
    // Catalog data dependencies
//    rc = catalog_data_dependencies(&model_, &functionStateGraph);
    // Catalog data io dependencies
//    rc = catalog_dataio_dependencies(&model_, &functionStateGraph);
    // Calculate task list using dependencies
//    rc = calculate_task_list(&tasks_);

#ifdef TESTBUILD
    // Output function dependency graph to view via graphviz dot
    // functionStateGraph.write_dependency_graph("dgraph.dot");
//    functionStateGraph.write_graphviz();
#endif

    return 0;
}

std::string ModelManager::taskTypeToString(Task::TaskType t) {
    /* Convert Task task to printable string */
    if (t == Task::io_pop_write) return "disk";
    else if (t == Task::sync_finish) return "comm";
    else if (t == Task::sync_start) return "comm";
    else if (t == Task::xfunction) return "func";
    /* If task not recognised return empty string */
    else
        return "";
}

void ModelManager::printTaskList(std::vector<Task*> * tasks) {
    std::vector<Task*>::iterator task;

    fprintf(stdout, "Level\tPriority\tType\tName\n");
    fprintf(stdout, "-----\t--------\t----\t----\n");
    for (task = tasks->begin(); task != tasks->end(); ++task) {
        fprintf(stdout, "%u\t%u\t\t%s\t%s\n",
                static_cast<unsigned int>((*task)->getLevel()),
                static_cast<unsigned int>((*task)->getPriorityLevel()),
                taskTypeToString((*task)->getTaskType()).c_str(),
                (*task)->getFullName().c_str());
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
