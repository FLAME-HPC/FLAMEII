/*!
 * \file FILENAME
 * \author Shawn Chin
 * \date 2012
 * \copyright Copyright (c) 2012 STFC Rutherford Appleton Laboratory
 * \copyright Copyright (c) 2012 University of Sheffield
 * \copyright GNU Lesser General Public License
 * \brief DESCRIPTION
 */
#ifndef MB__MESSAGE_ITERATOR_BACKEND_RAW_HPP_
#define MB__MESSAGE_ITERATOR_BACKEND_RAW_HPP_
#include <map>
#include <string>
#include "mb_common.hpp"
#include "message_iterator_backend.hpp"
namespace flame { namespace mb {

class MessageIteratorBackendRaw : public MessageIteratorBackend {
  public:
    MessageIteratorBackendRaw(MemoryMap* vec_map_ptr, TypeValidator *tv);
    bool AtEnd(void) const;
    size_t GetCount(void) const;
    bool Step(void);
    void Rewind(void);

    //! Returns false. Raw iterator is not mutable
    bool IsMutable(void) const { return false; }

    //! Returns a mutable version of this iterator
    // Handle GetMutableVersion(void);

  protected:

    void* Get(std::string var_name);

  private:
    typedef std::map<std::string, void*> RawPtrMap;
    TypeValidator* validator_;  //! Ptr to object used to validate datatypes
    RawPtrMap raw_map_; //! Map of raw pointers of vars
};

}}  // namespace flame::mb
#endif // MB__MESSAGE_ITERATOR_BACKEND_RAW_HPP_
