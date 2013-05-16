Module - EXE (Task Scheduling and Execution) {#modexe}
==================================

[TOC]

Tasks {#modexe-task}
======

(... what a task contains. diff between different Task subclasses, etc)

(... how tasks get access to agent memory and message boards)


Task Manager {#modexe-taskmgr}
============

The [TaskManager](@ref flame::exe::TaskManager) is a singleton object that handles 
the creation, storage, and indexing of all tasks. It also assists in the 
[scheduling of tasks](@ref modexe-sched) by performing *dependency accounting* 
(i.e. keeping track of fulfilled and pending dependencies for all tasks) and providing
the methods that the `TaskScheduler` uses to update dependency information and 
retrieve tasks.

Task creation and indexing {#modexe-taskmgr-create}
---------------------------

The `TaskManager` provides factory methods that should be used to create all tasks, e.g. 
[CreateAgentTask](@ref flame::exe::TaskManager::CreateAgentTask), 
[CreateMessagBoardTask](@ref flame::exe::TaskManager::CreateMessageBoardTask), 
and [CreateIOTask](@ref flame::exe::TaskManager::CreateIOTask). These methods 
instantiate the tasks using the appropriate `Task` subclass and exposes arguments
that are relevant to the said tasks.

Once a `Task` instance is created, it is stored within a boost::ptr_vector which 
automatically manages the lifespan of the object. The offset of the task within the 
vector is used as the task id -- this gives us an integer-based identifier which is
efficient to store and lookup.

On the user-level, tasks are identified by a string-based name; we therefore also store
a map (see flame::exe::TaskNameMap) which maps the task name to its integer-based ID. 
This mapping also serves as a quick way to detect duplicate task names on registration.

@img{images/taskmapping.png, 15cm, Mapping of task id and task name to instances}

Calls to flame::exe::TaskManager::GetTask using a `task_id` parameter can be performed
efficiently since we simply obtain the `Task` pointer using the id as the vector offset.
Calls using a string-based `task_name` parameter are less performant since it requires
lookup up the name map for the task id before we can retrieve the `Task` pointer.


Dependency accounting {#modexe-taskmgr-deps}
---------------------

At present, the `TaskManager` also keeps track of pending and fulfilled dependencies 
for all tasks throughout an iteration. This essentially controls the traversal of 
the dependency tree by ensuring that only *ready* tasks are available for enqueueing.
The dependency accounting data is reset at the end of each iteration to prepare for 
the next traversal of the tree.

The main methods that are involved in dependency accounting are:
 * [AddDependency()](@ref flame::exe::TaskManager::AddDependency) - assign a task 
    dependency (used during the initialisation stage).
 * [IterTaskAvailable()](@ref flame::exe::TaskManager::IterTaskAvailable) - check if there 
    are tasks ready for execution
 * [IterCompleted()](@ref flame::exe::TaskManager::IterCompleted) - check if all tasks 
    have been executed
 * [IterTaskPop()](@ref flame::exe::TaskManager::IterTaskPop) - retrieve a task that is 
    ready for execution
 * [IterTaskDone()](@ref flame::exe::TaskManager::IterTaskDone) - registers a completed 
    tasks. This will update the table of fulfilled
    dependencies and potentially migrated tasks to the ready queue.
 * [IterReset()](@ref flame::exe::TaskManager::IterReset) - resets the accounting data 
    in preparation for the next iteration.

The intended user for these methods is the flame::exe::Scheduler::RunIteration. This
interaction is descripted in the next section.

For implementation details, see:
 * flame::exe::TaskManager
 * flame::exe::Task

Task Scheduling {#modexe-sched}
===============

The activation and scheduling of tasks is achieved through the interation between the
[Scheduler](@ref flame::exe::Scheduler) and the [TaskManager](@ref flame::exe::TaskManager).
At the beginning of an iteration, the Scheduler is populated with tasks that are ready for
immediate execution (no dependencies) after which the completion of each task triggers
the execution of callback functions that marks fulfilled dependencies and enqueues
further tasks that have all dependencies met. 

It should be noted that the process is completely governed by the registered 
dependencies of the tasks -- **a non-conforming dependency graph (e.g. a cyclic graph) will
result in an iteration that never completes**. 

@img{images/exe_sched_flow.png, 15cm, Flowchart of operations achieved by the interaction between the Scheduler and TaskManager}


The actual scheduling and execution of tasks is handled by 
[TaskQueues](@ref flame::exe::TaskQueue). Each queues registered with the Scheduler
will be assigned a [Task type](@ref flame::exe::Task::TaskType) and this determines how
enqueued tasks are routed to the appropriate `TaskQueue`.

Each `TaskQueue` will contain one or more [WorkerThreads](@ref flame::exe::WorkerThread)
whereby the number of worker threads determines the number of slots it has, i.e. the number 
of concurrent tasks it can execute. Each `TaskQueue` may use a different scheduling 
algorithm -- e.g. FIFO or Priority-driven -- to assign enqueued tasks to worker threads. 
Some `TaskQueues` can perform task splitting, i.e. splitting up a large task into smaller
ones so the load can be spread more evenly across all worker threads (see 
flame::exe::SplittingFIFOTaskQueue).

@img{images/exe_sched.png, 15cm, Different queues within the scheduler allow concurrent operations with minimal locking}

The different queues and scheduling mechanisms allow the framework to better utilise the
resources available. For example, a typical setup could be to use the following set of
`TaskQueues`: 
 * priority-driven queue for CPU-intensive tasks (such as agent functions) with 
 the number of worker threads matching the number of CPU cores
 * FIFO queue for I/O operations (e.g. writing output data to disk) with the number of 
 threads matching the maximum number of simultaneous writes that the I/O subsystem can 
 handle efficiently
 * FIFO queue for communication operations (e.g. message board syncs) with the number of
 threads determining the maximum number of simultaneous communications. 
 

For implementation details, see:
 * flame::exe::TaskManager
 * flame::exe::Scheduler
 * flame::exe::TaskQueue
 * flame::exe::SplittingFIFOTaskQueue
 

Worker Threads {#modexe-worker}
==============

Each `TaskQueue` that requires asynchronous worker threads will hold a vector of 
[WorkerThread](@ref flame::exe::WorkerThread) objects. The number of instances is 
determined by the number of slots requested.

The `WorkerThread` class uses boost::thread to implement threading in a portable way. 
Once the thread starts, it goes into continuous loop of waiting for requesting a task
from the parent queue and executing flame::exe::Task::Run. A callback function of the
queue is issued after each run to indicate the completion of a task.

This process is repeated until the
`Term` task is issued (see flame::exe::Task::IsTermTask) after which the loop is broken
and the thread terminates. 

@img{images/exe_worker_flow.png, 10cm, Lifecycle of a worker thread}

In a standard implementation, the `Term` tasks are added to the queue by the destructor
of the `TaskQueue` object just before waiting for all worker threads to join 
(for example, see implementation of flame::exe::FIFOTaskQueue::~FIFOTaskQueue). The 
termination of threads is therefore done automatically when the `TaskQueue` gets 
destroyed, but only when all enqueued tasks have been executed.

For implementation details, see:
 * flame::exe::WorkerThread
 * flame::exe::FIFOTaskQueue
 * flame::exe::Task