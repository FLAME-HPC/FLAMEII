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
    explicit DataDependencyAnalyser(XGraph * graph, std::string name);
    void addDataDependencies(boost::ptr_vector<XVariable> * variables);

  private:
    XGraph * graph_;
    std::string name_;
    void addWriteDependencies(Vertex v, ModelTask * t);
    void addReadDependencies(Vertex v, ModelTask * t);
    void addWritingVerticesToList(Vertex v, ModelTask * t);
    void copyWritingAndReadingVerticesFromInEdges(Vertex v, ModelTask * t);
    void addStartTask(boost::ptr_vector<XVariable> * variables);
    void addEndTask();
};

} /* namespace model */
} /* namespace flame */
#endif  // MODEL__DATA_DEPENDENCY_ANALYSER_HPP
