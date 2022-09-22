---
title: IDATT2202, Assignment 2
author: Jakob Grønhaug (jakobkg@stud.ntnu.no)
---

# 1. Processes and threads

## 1.1 Differences

The biggest practical difference between processes and threads is that processes are isolated, do not share address space with each other and are not able to interrupt each other's execution. This is in contrast to threads, which share address space and are able to interrupt each other. All processes have dedicated stack and heap memory regions, while threads have dedicated stacks but shared heaps.

## 1.2 Use cases

In a case where having multiple running tasks access the same data, using threads is desireable as the shared data can be stored on the shared heap. For example, most sanely-written GUI application will have separate threads for work and GUI rendering, which both access the application state (work might mutate this, GUI rendering should only read state).

On the other hand, using separate processes for concurrency is a good idea when shared data is either unneeded or actively undesired. For web server processing, threads even having theoretical access to each other's heap data might pose a security or privacy risk and might be considered unwanted.

## 1.3 TCBs

Each thread in a process requires a separate thread control block (TCB) since they are independently executed and may be scheduled on/off the CPU at different times and as such need to be separately managed with separate register states and runtime relevant metadata.

## 1.4 Voluntary and involuntary threading

There are two ways in which a thread might be offloaded from the CPU to allow other threads to do work, voluntarily and involuntarily. A thread might voluntarily yield the CPU back to other threads if they know they're about to not need to do work for a while, such as "I just requested a bunch of data from disk, now I have to wait for that data to be loaded". A thread might also involuntarily be moved from a running state to a waiting or ready state if the scheduler decides it's time for a different process to do work on the CPU, or the thread could be unexpectedly ended due to an exception or interrupt.

### Voluntary context switch

- Save the thread's registers to thread stack
- Switch to a new thread and its stack
- Restore CPU registers from the newly loaded stack
- Return execution to the newly loaded thread

### Involuntary context switch

- An interrupt occurs, for example from I/O or clock hardware
- The thread's execution state is saved to its stack
- The interrupt handler is ran
- This handler might perform another context switch to hand the CPU over to the kernel
- Kernel thread execution state is loaded
- Kernel thread runs

# 2. C program with POSIX threads

## 2.1 Code executed in spawned threads

The function `go(void* n)` is ran on every new thread. This functin takes a pointer to a number `n`, prints "`Hello from thread n`" and returns the value `100 + n`.

## 2.2 Varying print order

The order in which "`Hello from thread n`" is printed is different each time the code is ran, this is because the threads that are created are not necessarily ran in the order in which they are created. Instead, the order of execution is decided by the whims of the scheduler.

## 2.3 Number of threads

When "`Hello from thread 8`" is printed, anywhere from 2 to 10 threads might exist. Thread 8 might be the first to print, in which case no threads have been joined yet and there are 10 threads (plus the main thread) in existence. However, thread 8 could also print after threads 0 through 7 have already printed and returned, in which case only threads 8 and 9 are the only thredas remaining (in addition to the main thread).

## 2.4 `pthread_join`

The function `pthread_join(thread_handle, return_value)` waits for a given thread to finish its work and return, and then reads the returned value, if any, from the thread into the heap address pointed to by the `return_value` pointer. In `threadHello`, this function is used to wait for the ten threads that have been spawned to return one by one.

Note that since the calls to `pthread_join()` are done in order from thread 0 to thread 10, the "`Thread n returned with (n + 100)`" prints will always appear in the same order.

## 2.5 Changing `go()`

With the presented changes to `go()`, all the prints of "`Hello from thread n`" would happen as usual, but "`Thread n returned...`" would only reach thread 4 and then halt until thread 5 has finished sleeping for 2 seconds before proceeding with threads 5 through 9 once thread 5 finishes sleeping and exits.

## 2.6 Thread states when joining

A thread is only joined by `pthread_join()` once it has called `pthread_exit()`, meaning the thread is in a Finished state.
