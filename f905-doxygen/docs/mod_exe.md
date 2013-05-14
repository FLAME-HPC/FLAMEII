Module - EXE (Task Scheduling and Execution) {#modexe}
==================================

[TOC]


Tasks {#modexe-task}
======

(... what a task contains. diff between different Task subclasses, etc)

(... mapping task names to agent functions -- task registration) 

(... how tasks get access to agent memory and message boards)



Task Scheduling {#modexe-sched}
===============

(.. scheduler relies on different queues to perform actual task scheduling. Routes to
appropriate queues. Slots in each queue determines the level of concurrency)

(.. interacts with TaskManager to retrieve tasks ready for execution. Callback functions
drives the dependency calculation within the task manager)

![Different queues within the scheduler allow concurrent operations with minimal locking](images/exe_sched.png)

![Flowchart of operations achieved by the interaction between the Scheduler and TaskManager](images/exe_sched_flow.png)


Worker Threads {#modexe-worker}
==============
![Lifecycle of a worker thread](images/exe_worker_flow.png)

