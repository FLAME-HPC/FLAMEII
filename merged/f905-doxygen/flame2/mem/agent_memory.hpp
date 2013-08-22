/*!
 * \file flame2/mem/agent_memory.hpp
 * \author Shawn Chin
 * \date 2012
 * \copyright Copyright (c) 2012 STFC Rutherford Appleton Laboratory
 * \copyright Copyright (c) 2012 University of Sheffield
 * \copyright GNU Lesser General Public License
 * \brief AgentMemory: management and storage class for per-agent memory vectors
 */
#ifndef MEM__AGENT_MEMORY_HPP_
#define MEM__AGENT_MEMORY_HPP_
#include <string>
#include <utility>  // for std::pair
#include <vector>
#include <typeinfo>
#include <boost/ptr_container/ptr_map.hpp>
#include "flame2/exceptions/mem.hpp"
#include "vector_wrapper.hpp"

// TODO(lsc): review usage of AgentMemory::registration_closed_
// Do we need it? Is there a better way to handle access to RegisterVar?

namespace flame { namespace mem {

namespace exc = flame::exceptions;

//! Map container used to store memory vectors
typedef boost::ptr_map<std::string, VectorWrapperBase> MemoryMap;


//! Container for memory vectors associated with an agent type
class AgentMemory {
  public:
    /*!
     * \brief constructor
     * \param agent_name agent name
     */
    explicit AgentMemory(const std::string& agent_name)
        : agent_name_(agent_name), mem_map_(),
#ifdef DEBUG
          cached_size_(0),
#endif
          registration_closed_(false) {}

    /*! 
     * \brief Registers a memory variable of a specific type
     * \param var_name variable name
     */
    template <typename T>
    void RegisterVar(std::string var_name) {
      if (registration_closed_) {
        throw exc::logic_error("variables can no longer be registered");
      }
      std::pair<MemoryMap::iterator, bool> ret;
      ret = mem_map_.insert(var_name, new VectorWrapper<T>());
      if (!ret.second) {  // key exists. No insertion
        throw exc::logic_error("variable already registered");
      }
    }

    /*!
     * \brief indicate likely population size
     * \param size_hint size hint
     *
     * Hint at a population size so required memory can be reserved
     * in advance. This saves having to constantly reallocate memory
     * as agents are added to AgentMemory.
     */
    void HintPopulationSize(unsigned int size_hint);

    /*!
     * \brief Returns the current population size
     * \return population size
     *
     * For now, we query the size from the first memory vector. In the future,
     * once we have a more structured approach to populating the vectors, we
     * should then simply return the counter value
     *
     * In debug more, we double check the size against all other memory vectors.
     * To speed up subsequent calls, we cache the return value and skip the
     * checks if the size remains the same.
     *
     * Throws flame::exceptions::flame_mem_exception when the sizes are inconsistent
     *
     * Not that these checks will not detect problems when other vectors apart from
     * the first reduces in size.
     *
     */
    size_t GetPopulationSize(void);

    /*! 
     * \brief Returns typeless pointer to associated vector wrapper
     * \param var_name variable name
     * \return pointer to VectorWrapper instance
     */
    VectorWrapperBase* GetVectorWrapper(const std::string& var_name);

    /*! 
     * \brief retrieve pointer to underlying data vector 
     * \param var_name variable name
     * \return pointer to the typed vector instance
     */
    template <typename T>
    std::vector<T>* GetVector(const std::string& var_name) {
      VectorWrapperBase* ptr;
      registration_closed_ = true;  // no more new variables

      try {
        ptr = &(mem_map_.at(var_name));
      }
      catch(const boost::bad_ptr_container_operation& E) {
        throw exc::invalid_variable("Invalid agent memory variable");
      }

#ifndef DISABLE_RUNTIME_TYPE_CHECKING
      if (*(ptr->GetDataType()) != typeid(T)) {
        throw exc::invalid_type("Invalid data type specified");
      }
#endif
      return static_cast<std::vector<T>*>(ptr->GetVectorPtr());
    }

    /*! 
     * \brief Query if said memory variable has been registered.
     * \param var_name variable name
     * \return true if memory variable has been registered
     */
    bool IsRegistered(const std::string& var_name) const;

  private:
    std::string agent_name_;  //!< Name of agent
    MemoryMap mem_map_;  //!< Map of var names to VectorWrapper
#ifdef DEBUG
    size_t cached_size_;  //!< Cache of size
#endif

    //! Indicates that vectors have been resized/populated so new variables
    //!  should no longer be registered.
    bool registration_closed_;
};
}}  // namespace flame::mem
#endif  // MEM__AGENT_MEMORY_HPP_
