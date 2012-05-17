/*!
 * \file src/mem/agent_shadow.hpp
 * \author Shawn Chin
 * \date 2012
 * \copyright Copyright (c) 2012 STFC Rutherford Appleton Laboratory
 * \copyright Copyright (c) 2012 University of Sheffield
 * \copyright GNU Lesser General Public License
 * \brief Proxy object which only exposes selected vars of an agent
 */
//! TODO(lsc): since the number of entries in vec_map_ and rw_set_ will be
//! relatively small, the tree-based search used by std::map and
//! std::set may not be ideal. Consider implementing variants that
//! uses a sorted vector instead. See:
//!  - AssocVector: http://loki-lib.sourceforge.net/html/a00645.html
//!  - http://lafstern.org/matt/col1.pdf
//!  - http://www.codeproject.com/Articles/27799/Stree

//! TODO(lsc): Support task splitting. This involves:
//!  - Creating new MemoryIterators that can be stepped through independently
//!  - Using a counter to detect end-of-vector instead of vector::end()
//!  - ptr_map_ pointing to an offset within a raw array
//!  - Rewind() needs to reset to the correct offset, not vector::start()

#ifndef MEM__AGENT_SHADOW_HPP_
#define MEM__AGENT_SHADOW_HPP_
#include <map>
#include <set>
#include <vector>
#include <string>
#include <utility>
#include "boost/shared_ptr.hpp"

namespace flame { namespace mem {

class AgentMemory;  // forward declaration
class MemoryIterator; // forward declaration
class VectorWrapperBase;  // forward declaration

typedef std::map<std::string, VectorWrapperBase* const> ConstVectorMap;
typedef std::pair<std::string, VectorWrapperBase* const> ConstVectorMapValue;
typedef boost::shared_ptr<MemoryIterator> MemoryIteratorPtr;
typedef std::set<std::string> WriteableSet;

class AgentShadow {
  friend class MemoryManager;
  friend class MemoryIterator;

  public:
    void AllowAccess(const std::string& var_name, bool writeable = false);
    size_t get_size() const;
    MemoryIteratorPtr GetMemoryIterator();

  protected:
    explicit AgentShadow(AgentMemory* am);
    WriteableSet rw_set_;  //! Set of var names that has write access
    ConstVectorMap vec_map_;  //! map accessible vars

  private:
    size_t size_;  //! Size if memory vectors
    AgentMemory* am_;

    AgentShadow(const AgentShadow&);  //! Disable copy ctor
    void operator=(const AgentShadow&);  //! Disable assignment
};

}}  //  namespace flame::mem
#endif  // MEM__AGENT_SHADOW_HPP_
