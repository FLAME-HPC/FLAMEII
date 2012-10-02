/*!
 * \file src/mb/mb_common.hpp
 * \author Shawn Chin
 * \date September 2012
 * \copyright Copyright (c) 2012 STFC Rutherford Appleton Laboratory
 * \copyright Copyright (c) 2012 University of Sheffield
 * \copyright GNU Lesser General Public License
 * \brief Datatypes common to flame::mb
 */
#include <map>
#include <string>
#include "boost/shared_ptr.hpp"
#include "boost/function.hpp"
#include "boost/ptr_container/ptr_map.hpp"
#include "mem/vector_wrapper.hpp"

#ifndef MB__MB_COMMON_HPP
#define MB__MB_COMMON_HPP
namespace flame { namespace mb {

// forward declarations
class Message;
class MessageIterator;
class BoardWriter;
class TypeValidator;

//! Handle to return in place of Message
typedef boost::shared_ptr<Message> MessageHandle;

//! Handle to return in place of BoardWriter
typedef boost::shared_ptr<BoardWriter> BoardWriterHandle;

//! Handle to return in place of a MessageIterator
typedef boost::shared_ptr<MessageIterator> MessageIteratorHandle;

//! Shorthand for VectorWrapper base class
typedef flame::mem::VectorWrapperBase GenericVector;

//! Map container used to store memory vectors
typedef boost::ptr_map<std::string, GenericVector> MemoryMap;

//! Function signature for callback function triggered by Message::Post()
typedef boost::function<void (Message*)> MessagePostCallback;

}}  // namespace flame::mb
#endif  // MB__MB_COMMON_HPP
