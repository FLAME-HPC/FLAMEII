Module - MB (Message Management) {#modmb}
==================================

[TOC]

Overview {#modmb-overview}
=========

The MB module handles the storage and access to message boards. All interaction with
the MB module should be done via the `MessageBoardManager`
-- a singleton instance of flame::mb::MessageBoardManager.

The most common `MessageBoardManager` methods are:

 * [MessageBoardManager::GetInstance()](@ref flame::mb::MessageBoardManager::GetInstance) -- 
   class method to returns a reference to the singleton
   `MessageBoardManager` instance. All methods should be run off this instance. 
 * [RegisterMessage()](@ref flame::mb::MessageBoardManager::RegisterMessage) -- 
    registers a new agent type.
 * [GetBoardWriter()](@ref flame::mb::MessageBoardManager::GetBoardWriter) -- 
    returns a [writer](@ref modmb-writers) object to allow thread-safe message posting
 * [GetMessages()](@ref flame::mb::MessageBoardManager::GetMessages) -- 
    returns a message board [iterator](@ref modmb-iterator).
 * [Sync()](@ref flame::mb::MessageBoardManager::Sync) -- 
    synchronises the message board. This method is never run manually and should be
    run from a task launched by the scheduler.
 * [Clear()](@ref flame::mb::MessageBoardManager::Clear) -- 
    empties the message board. This method is never run manually and should be
    run from a task launched by the scheduler.
    

Data Storage {#modmb-storate}
============

At present there is only one MessageBoard class -- flame::mb::MessageBoard.
This is the most basic message board which uses [VectorWrapper](@ref flame::mem::VectorWrapper)
for storing data. Messages are therefore stored contiguous in memory, and message board 
instances will only be able to store messages of a specific pre-declared datatype.

To store messages with multiple variables, a `struct` should be used as the datatype.
Due to the [OutputToStream()](@ref flame::mem::VectorWrapper::OutputToStream) method
of `VectorWrapper<T>`, all datatypes stored within the current `MessageBoard` implementation
must therefore support the `operator<<` method.  This is available for all native 
datatypes but custom `struct`s must define that method manually. For example:

    // Datatype of our message
    typedef struct {
      double x, y;
      int id;
    } location_message;

    // The structure must support the << operator for it to be store in the board
    inline std::ostream &operator<<(std::ostream &os, const location_message& ob) {
      os << "{" << ob.x << ", " << ob.y << ", " << ob.id << "}";
      return os;
    }


We expect future extensions of the FLAME framework to expose an interface and 
the necessary class layout such that users can implement their own messageboard subclass 
(or storage backend). This will allow users to use custom data storage approaches that 
are more suitable for their data and usage pattern. 

For example, to store messages that contain positional coordinates that are used for
nearest-neighbour searches, the `MessageBoard` subclass could build a space-partitioned
data structure (e.g. k-d tree) to index messages that are stored. The building of the 
tree could be preformed during a `Sync()` call where the actual number of messages are
known and this would result in a well balanced tree.

`MessageBoard` implementations should not be limited to in-memory storage schemes. It 
can be used for more dynamic purposes such as as a front for any method, application,
or protocol that consumes and produces data. Some example usages are:

 * driving a real-time visualisation tool - agents post current location as messages and
   this is transmitted to the visualisation application
 * computational steering - agents read potential changes in runtime parameters from 
   messages which are populated by the CS application.
 * interface to external solvers - agents post parameters to a board and later reads the
   solution either from the same board if the output format is similar, or from another
   co-operating board. This is particularly handy for solvers that require information
   from all agents before the solution can be worked out.

It may be necessary to introduce, initialisation, finalisation and  pre/post `Sync()` 
hooks that subclasses can implement in order to support more usage scenarios.

For implementation details, see:
 * flame::mb::MessageBoard
 * flame::mem::VectorWrapper


Board Writers {#modmb-writers}
=============

To minimise the overheads of locking when there are concurrent threads writing to the
board, we introduce the concept of [BoardWriter](@ref flame::mb::BoardWriter)s. 

Messages are never posted directly into the storage backend of a `MessageBoard`. Instead,
threads that need write access to the board has to request for a 
`BoardWriter` instance. Write access to the `BoardWriter` instance is not thread-safe
and it is the developers' responsibility to ensure that each instance is assigned to
only one thread.

A `BoardWriter` acts as a staging area for messages. When a `MessageBoard` is 
[synchronised](@ref flame::mb::MessageBoard::Sync), messages from all writes are 
transferred into the board after which the writes are invalidated and marked for 
deletion. Threads should therefore cache writers only for the duration of a task 
execution (in the current implementation, writers are cached within 
flame::mb::Client which are instantiated using flame::mb::Proxy::GetClient).

For implementation details, see:
 * flame::mb::BoardWriter
 * flame::mb::MessageBoard
 
 
Board Iterators {#modmb-iterator}
===============

The more efficient way to iterate through messages would be to directly refer to the
array of messages that are store contiguous in memory within the `VectorWrapper` instance.
However, this approach would mean that the messages must be iterated in-order which may
not be a case for filtered or randomised access.

To allow for both access methods, [MessageIterator](@ref flame::mb::MessageIterator) 
contain [backends](@ref flame::mb::MessageIteratorBackend) 
(which perform the actual iteration) that can be swapped out when a different access mode 
is required. 

For instance, a standard iterator would start off using a backend that contain a raw 
pointer to the underlying vectors 
([MessageIteratorBackendRaw](@ref flame::mb::MessageIteratorBackendRaw)). 
This raw pointer is retrieved using 
[VectorWrapper::GetRawPtr()](@ref flame::mem::VectorWrapper::GetRawPtr) and 
stepped using [VectorWrapper::StepRawPtr()](@ref flame::mem::VectorWrapper::StepRawPtr)
so the memory step sizes are dealt with automatically.

@img{images/iterator_raw.png, 15cm, Raw iterator backends directly access memory vectors}

When randomisation or sorting is required for the iterator, the backend is
first swapped out for a mutable version (`MessageIteratorBackendMutable`), i.e. one that
stores a vector of indices of messages. The indices vector indicate the order in which
the message vector is accessed and can be mutated to allow partial or out-of-order iteration.

@img{images/iterator_mutable.png, 15cm, Mutable iterator stores a list of indices to allow partial/unordered access}

Naturally, iterators that contain mutable backends are less efficient as there are extra
layers of indirection for each access.

It is possible to write backends that are a compromise between the *raw* and *mutable*
version for very specialised access patterns, e.g. iterators that continuosly return
random entries from the board, strided access, etc. As these are very niche use cases,
we leave it up to power users to implement their own subclasses for it. The FLAME-II
interfaces and APIs should be designed to allow for this.

For implementation details, see:
 * flame::mb::MessageIterator
 * flame::mb::MessageIteratorBackend
 * flame::mb::MessageIteratorBackendRaw
 * flame::mb::MessageIteratorBackendMutable (not yet implemented)
 

API Access to Message Boards (access control) {#modmb-api-acl}
=============================================

Message boards are never accessed directly by the APIs. Instead, they are
accessed through methods of a flame::mb::Client object. This object stores
a set of message names that the client can read from, and a map of
[BoardWriters](@ref flame::mb::BoardWriter) that the client can write to.

Considering the relative low number of messages that will be access per
agent function, boost::container::flat_set and boost::container::flat_map
are used instead of std::set and std::map to improve access speed.
For a discussion on this, see http://lafstern.org/matt/col1.pdf.

The contents of the set/map are populated when the Client object is
instantiated using the [Proxy](@ref flame::mb::Proxy) object.

The [Proxy](@ref flame::mb::Proxy) is essentially a
[Client](@ref flame::mb::Client) factory -- it is attached to any
[Task](@ref flame::exe::Task) instance that needs access to message boards
and stores a list message names that can be read from from posted to.
Access is granted by calling the
[Proxy::AllowRead](@ref flame::mb::Proxy::AllowRead) and
[Proxy::AllowPost](@ref flame::mb::Proxy::AllowPost) methods. 

For implementation details, see:
 * flame::mb::Client
 * flame::mb::Proxy
