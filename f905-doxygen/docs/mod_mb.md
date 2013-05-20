Module - MB (Message Management) {#modmb}
==================================

[TOC]

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
 
Message Board Manager {#modmb-manager}
=====================


API Access to Message Boards (access control) {#modmb-api-acl}
=============================================

(Details about Client and Proxy) 