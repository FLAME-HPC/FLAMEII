/*!
 * \file flame2/model/data_dependency_analyser.hpp
 * \author Simon Coakley
 * \date 2013
 * \copyright Copyright (c) 2013 STFC Rutherford Appleton Laboratory
 * \copyright Copyright (c) 2013 University of Sheffield
 * \copyright GNU Lesser General Public License
 * \brief DataDependencyAnalyser: analyses data dependency
 */
#ifndef MODEL__DATA_DEPENDENCY_ANALYSER_HPP
#define MODEL__DATA_DEPENDENCY_ANALYSER_HPP
#include <string>
#include <boost/ptr_container/ptr_vector.hpp>
#include "xvariable.hpp"
#include "model_task.hpp"
#include "xgraph.hpp"

namespace flame {
namespace model {

class DataDependencyAnalyser {
  public:
    //! Constructor requires graph and graph name
    explicit DataDependencyAnalyser(XGraph * graph, std::string name);
    //! Add all data dependencies on variables to the graph
    void addDataDependencies(boost::ptr_vector<XVariable> * variables);

  private:
    //! The graph
    XGraph * graph_;
    //! The graph name (added to new tasks)
    std::string name_;
    //! Add dependencies on reading vertices that the task writes
    void addWriteDependencies(Vertex v, ModelTask * t);
    //! Add dependencies on writing vertices that the task reads
    void addReadDependencies(Vertex v, ModelTask * t);
    //! Add vertices to variable writing vertices list for writing variables
    void addWritingVerticesToList(Vertex v, ModelTask * t);
    //! Copy writing and reading variable lists from tasks that the
    //! current task depends
    void copyWritingAndReadingVerticesFromInEdges(Vertex v, ModelTask * t);
    //! Add start task that writes all variables so that if any variables
    //! are not subsequently written they will be at the start
    void addStartTask(boost::ptr_vector<XVariable> * variables);
    //! Add end task that holds all the tasks that last write each variable
    void addEndTask();
};

} /* namespace model */
} /* namespace flame */
#endif  // MODEL__DATA_DEPENDENCY_ANALYSER_HPP
