/*!
 * \file flame2/mem/agent_shadow.hpp
 * \author Shawn Chin
 * \date 2012
 * \copyright Copyright (c) 2012 STFC Rutherford Appleton Laboratory
 * \copyright Copyright (c) 2012 University of Sheffield
 * \copyright GNU Lesser General Public License
 * \brief Proxy object which only exposes selected vars of an agent
 */

#ifndef MEM__AGENT_SHADOW_HPP_
#define MEM__AGENT_SHADOW_HPP_
#include <map>
#include <set>
#include <vector>
#include <string>
#include <utility>
#include <boost/shared_ptr.hpp>

namespace flame { namespace mem {

class AgentMemory;  // forward declaration
class MemoryIterator;  // forward declaration
class VectorWrapperBase;  // forward declaration

//! Map type used to store pointers to VectorWrappers
typedef std::map<std::string, VectorWrapperBase* const> ConstVectorMap;
//! Set type used to keep track of vars with write permission
typedef std::set<std::string> WriteableSet;
//! Smart pointer type used to return MemoryIterator
typedef boost::shared_ptr<MemoryIterator> MemoryIteratorPtr;

//! \brief Proxy object which only exposes selected vars of an agent
//! \todo since the number of entries in vec_map_ and rw_set_ will be
//!       relatively small, the tree-based search used by std::map and std::set
//!       may not be ideal.
//!       Consider using boost::flat_map and boost::flat_set
//!         (see flame::mb::writer_map_type).
class AgentShadow {
  friend class MemoryManager;
  friend class MemoryIterator;

  public:
    /*! 
     * \brief Enables access to an agent variable
     * \param var_name variable name
     * \param writeable should the variable be writeable (default = false)
     */
    void AllowAccess(const std::string& var_name, bool writeable = false);

    /*! 
     * \brief Query the population size
     * \return population size
     */
    size_t get_size();

    /*! 
     * \brief Retrieve a new instance of a MemoryIterator
     * \return pointer to memory iterator
     */
    MemoryIteratorPtr GetMemoryIterator();

    /*! 
     * \brief Retrieve a new instance of a MemoryIterator which targets only a
     *        subset of memory
     * \param offset memory offset
     * \param count number of messages addressed by iterator
     * \return pointer to memory iterator
     */
    MemoryIteratorPtr GetMemoryIterator(size_t offset, size_t count);

  protected:
    /*!
     * \brief protected constructor. Accessible only by MemoryManager.
     * \param am agent memory
     */
    explicit AgentShadow(AgentMemory* am);
    // Accessible to MemoryIterator
    WriteableSet rw_set_;  //!< Set of var names that has write access
    // Accessible to MemoryIterator
    ConstVectorMap vec_map_;  //!< map accessible vars

	/*!
	 * \brief Query if variable name is registered
	 * \param \var_name variable name
	 * \return true if registered
	 */
    bool IsRegistered(const std::string& var_name) const;

  private:
    // size_t size_;  //!< Size if memory vectors
    AgentMemory* am_;  //!< Pointer to parent AgentMemory object

    AgentShadow(const AgentShadow&);  //!< Disable copy ctor
    void operator=(const AgentShadow&);  //!< Disable assignment
};

}}  //  namespace flame::mem
#endif  // MEM__AGENT_SHADOW_HPP_
