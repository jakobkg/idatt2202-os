---
title: IDATT2202, Assignment 3
author: Jakob Grønhaug (jakobkg@stud.ntnu.no)
numbersections: true
geometry: margin=2cm
---

# Synchronization

1. 
   An example of inter-process communication would be the system settings menu telling the underlying system to change a setting such as audio output device based on user input, or the spell checking thread of a word processor app letting the main thread know to put red squigglies under a misspelled word. This communication can be done via shared memory, sockets, signals, among others.
   
   Different means of inter-process communication can have different issues. Directly sharing memory bypasses the performance overhead of having the kernel act as a mediator but also opens the door to issues such as race conditions, where the outcome of a given process is determined by the order in which different threads and their instructions are executed, which could non-deterministic based on the whims and fancies of the scheduler. Other means of inter-process communication such as sockets provide solutions to these issues, at the cost of implementation complexity and overhead as the kernel handles the communication between the processes.

2. 
   The critical region is the part of a program that in some way accesses a shared resource and is at risk of running into synchronization issues, since whether a different thread or process is currently accessing the same shared resource might not be known without using some sort of locking mechanism that ensures only one thread is allowed to access the shared resource at a time. A user-mode process can always be interrupted, even in a critical section of code. However, in kernel code it's common to temporarily disable interrupts while in a critical section to ensure these bits of code have as few sources of non-determinism as possible.

3. 
   Busy waiting is the programming equivalent of "are we there yet are we there yet are we there yet", where a process checks the availability of a resource as often as possible with a loop such as `while (resource is busy) { check if resource is available }`. On the other hand, blocking/sleep waiting means that a process checks whether a shared resource is available and goes to sleep if the resource is busy, effectively yielding the processor until the thread spuriously wakes back up to perform another check or is awakened by a signal that the resource is now available to use. Busy waiting is easy to implement and guarantees mutual exclusion from a resource, but if waiting times are not insignificant busy waiting will waste CPU time and should be considered a crime against the environment.

4. 
   A race condition is an undesired state where the outcome of one process depends on the execution state of another, and the state of this other process is not guaranteed by some sort of synchronization mechanism. For example, a banking system needs to implement protections against race conditions when dealing with transactions to ensure that no transaction is accidentally overwritten
   
    Let's consider a banking server, which spawns new threads to handle every incoming transaction request. Bob has 100kr in their account, and is at the store buying some milk. As Bob pays using their card, a request is sent to the bank's server to deduct 20kr from Bob's account. At the same time, Bob's friend sends Bob 50kr via Vipps in order to pay Bob back after a café visit earlier. If these two transactions are not handled properly, a race condition might occur in which both the transaction processes read Bob's account balance as 100kr at the start of their critical sections. The deduction process then writes 80kr back to Bob's account, and the vipps process overwrites this result with its own result of 150kr. Free milk! Determining whether free milk is a desireable outcome is left as an exercise to the reader, but $100 - 20 + 50 \neq 150$ so we can safely state that at a technical level this is a problem.

5. 
   A spin-lock is a type of lock where the waiting time can be assumed to be short and busy waiting might be used. This type of locking mechanism is often found in kernel code to share data between kernel threads where waiting times are known to be short.

6. 
   - In multi-core architectures, synchronizing state and data between processes becomes more complex as processes running on different cores with separate caches still need synchronization mechanisms and means of syncing data. This causes additional overhead as the shared data needs to be copied to and from the caches of different cores independently of the regular caching algorithms of the system.
   - MCS is a type of spinlock mechanism well suited for situations with many waiting threads, while RCU is well suited for situations where many threads are trying to read from a shared resource, but writes to the resource are comparatively rare by reducing overhead for reads while increasing overhead for writes.
   - At the heart of both of these is the idea of an atomic read-modify-write instruction like compare-and-swap or test-and-set (CMPXCHG and BTS respectively in x86)

# Deadlocks

1. 
   Starvation is the case where a thread fails to progress past a certain point for some length of time because the resources it needs are somehow inaccessible or locked. A deadlock is a specific case where two or more threads are starving each other so that neither may proceed with execution (e.g. a thread A is waiting for a thread B to unlock a resource, while thread B is also waiting for thread A to unlock a resource).
2. 
   - The kernel must be unable to take access to a resource away from a thread
   - A thread must be locking and waiting (a thread locks one resource, and then requests another one while still holding the first)
   - Circular locking (two or more threads are waiting for each other in a circual fashion)
   - There must be limited resources for a deadlock to occur

3. 
   The OS keeps track of the execution state of every thread, which resources each thread has locked to itself and which resources are currently being requested via system calls. If a thread A has a lock on resource X and requests resource Y, while a thread B has a lock on resource Y and then requests resource X, this becomes a detectable circular dependency where A is waiting for B, which is waiting for A.

# Scheduling
1. 
   FIFO scheduling is optimal when the queued tasks are roughly equal in
