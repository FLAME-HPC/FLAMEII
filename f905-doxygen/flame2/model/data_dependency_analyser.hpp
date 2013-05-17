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
#include <set>
#include <boost/ptr_container/ptr_vector.hpp>
#include "xvariable.hpp"
#include "model_task.hpp"
#include "xgraph.hpp"

namespace flame {
namespace model {

/*!
 * \brief Data dependency analyser called by graphs
 */
class DataDependencyAnalyser {
  public:
    /*!
     * \brief Constructor
     * \param[in] graph The graph
     * \param[in] name The graph name
     */
    explicit DataDependencyAnalyser(XGraph * graph, std::string name);
    /*!
     * \brief Add all data dependencies on variables to the graph
     * \param[in] variables All agent variables
     */
    void addDataDependencies(boost::ptr_vector<XVariable> * variables);

  private:
    //! The graph
    XGraph * graph_;
    //! The graph name (added to new tasks)
    std::string name_;
    /*!
     * \brief Add dependencies on reading vertices that the task writes
     * \param[in] v Vertex
     * \param[in] t Task
     */
    void addWriteDependencies(Vertex v, ModelTask * t);
    /*!
     * \brief Add dependencies on writing vertices that the task reads
     * \param[in] v Vertex
     * \param[in] t Task
     */
    void addReadDependencies(Vertex v, ModelTask * t);
    /*!
     * \brief Add vertices to variable writing vertices list for
     *        writing variables
     * \param[in] v Vertex
     * \param[in] t Task
     */
    void addWritingVerticesToList(Vertex v, ModelTask * t);
    /*!
     * \brief Copy writing and reading variable lists from tasks that the
     *        current task depends
     * \param[in] v Vertex
     * \param[in] t Task
     */
    void copyWritingAndReadingVerticesFromInEdges(Vertex v, ModelTask * t);
    /*!
     * \brief Add start task that writes all variables so that if any variables
     *        are not subsequently written they will be at the start
     * \param[in] variables Agent variables
     */
    void addStartTask(boost::ptr_vector<XVariable> * variables);
    /*!
     * \brief Add end task that holds all the tasks that last write each variable
     */
    void addEndTask();
    /*!
     * \brief Clear variable write set of vertices
     * \param[in] name Variable name
     * \param[in] lastWrites Map of variable names to vertices
     */
    void clearVarWriteSet(std::string name, VarMapToVertices * lastWrites);
    /*!
     * \brief Get vertex set associated with variable name
     * \param[in] name Variable name
     * \param[in] lastWrites Map of variable names to vertices
     * \return Set of vertices
     */
    std::set<size_t> * getVertexSet(
        std::string name, VarMapToVertices * lastWrites);
    /*!
     * \brief Add vertex to variable write set
     * \param[in] name Variable name
     * \param[in] v Vertex
     * \param[in] lastWrites Map of variable names to vertices
     */
    void addVertexToVarWriteSet(std::string name, Vertex v,
        VarMapToVertices * lastWrites);
    /*!
     * \brief Copy variable write set
     * \param[in] from Map to copy from
     * \param[out] to Map to copy to
     */
    void copyVarWriteSets(VarMapToVertices * from, VarMapToVertices * to);
};

} /* namespace model */
} /* namespace flame */
#endif  // MODEL__DATA_DEPENDENCY_ANALYSER_HPP
