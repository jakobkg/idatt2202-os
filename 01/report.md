---
title: IDATT2202 - Assignment 1
author: Jakob Grønhaug (jakobkg@stud.ntnu.no)
numbersections: true
---

# The process abstraction

## How a program is started

When a program is started from disk, the executable is first loaded from the disk to memory by the kernel. The kernel then performs a mode switch from kernel mode to user mode and begins execution at the entrypoint of the executable. This switch from kernel mode to user mode ensures that the code within the executable is only given limited access to memory and instructions, so that it can't read/write to the memory of other processes and can't do things like deferral of interrupts.

## The Linux kernel process structure

The file in the Linux kernel source tree that contains the process descriptor structure (`struct task_struct`) is `linux/sched.h`. This struct keeps the process ID of the represented process in the field `pid_t task_struct->pid`, and the accumulated virtual memory in `u64 task_struct->acct_vm_mem1`. The utility `top` also shows processes' executable names and priority, which are found in `char task_struct->comm[]` and `int task_struct->prio` respectively.

# Process memory and segments

## A process' address space

The address space of a process is roughly organized as follows:

- Stack
- Heap
- Data segment
- Text segment
- Reserved page
- 0x0

## What's what

- The stack is where data with a known, limited scope is typically stored. The stack is expanded whenever a new scope is created (e.g. when a function is called), and shrunk whenever a scope is closed (e.g. a function returns). As the name implies, data is always added to the top of the stack and removed from the top of the stack.
- The heap is where data not explicitly scoped or without a known lifetime is typically stored. Data on the heap is manually allocated using methods such as `malloc()` in C, and it's the job of the programmer to allocate and deallocate this data as opposed to the stack where data is deallocated once it goes out of scope.
- The data segment is where static and global data lives. In C, this is, among other things, data that's initialized in global scope i.e. outside of `main()`.
- The text segment contains the actual executable code in a program. This segment is often shared so that multiple running instances of the same program don't load duplicates of the same code into memory.
- At the bottom of the address space is a reserved page. This page is put here so that references to memory address 0x0 is never valid, and can be correctly identified as a null pointer exception.

## Types of variables

### Global

A *global* variable is a variable declared in the outermost scope, which makes it accessible to all other scopes.

For example: 

```c
int globalVar = 0;

void print_var() {
    // Accessible in this function scope
    printf("%d\n", globalVar);
}

void increment_var() {
    // Can even be mutated
    ++globalVar;
}

int main() {
    // Accessible in main's scope
    printf("%d\n", globalVar);  // Prints 0
    print_var();                // Prints 0
    increment_var();
    print_var();                // Prints 1
    return 0;
}
```

### Static

A *static* variable is a variable whose value is preserved even when its containing scope is closed, meaning it doesn't get re-initialized once it comes back into scope. For example:

```c
void static_print() {
    static int staticVar = 0;
    staticVar++;
    printf("%d\n", staticVar);
}

int main() {
    static_print(); // Prints 1
    static_print(); // Prints 2
}
```

### Local

A *local* variable is a variable with a limited scope. It goes into scope once declared, and is completely cleared once its scope is closed. If its containing scope is re-initialized, so are its local variables. For example:

```c
void local_print() {
    int localVar = 0;
    localVar++;
    printf("%d\n", localVar);
}

int main() {
    local_print(); // Prints 1
    local_print(); // Prints 1
}
```

### Snippet

In the provided snippet, `var1` is a global mutable variable which is stored in the data segment, `var2` is a value stored on the heap and `var3` is a pointer stored on the stack (pointing to a value stored on the heap).

# mem.c

## Sizes

After compiling the above snippet with `gcc`, the resulting binary has a text segment of 1249 bytes, a data segment of 540 bytes and a bss segment of 12 bytes according to the `size` utility

```
$ size mem
   text    data     bss     dec     hex filename
   1249     540      12    1801     709 mem
```

## Start address

The start address of the program is 0x0000000000401050

```
$ objdump -f mem

mem:     file format elf64-x86-64
architecture: i386:x86-64, flags 0x00000112:
EXEC_P, HAS_SYMS, D_PAGED
start address 0x0000000000401050
```

## Disassembly

The function at address 0x0000000000401050 is `_start`, which appears to be the OS's entrypoint into the program that does *something* and then calls `main()` via `__libc_start_main`

## Repeated runs

When the program is ran several times the addresses of the stack allocated variables change between runs due to Address Space Layout Randomization, a security feature intended to protect programs from one another.

# The stack

According to `ulimit -s`, the default process stack size of my system is 8192KB. Compiling and running the provided `stackoverflow` snippet causes quite a lot of lines to be printed into the terminal, ending with a segfault. The cause seems to be too many recursive calls (`func()` calls `func()`, which calls `func()` which...) causing the stack to fill up with copies of the local variable `localvar` before eventually overflowing, triggering the program to be terminated by the kernel for trying to write past its allocated stack space.

Counting the number of lines containing the string "func" with `wc -l` gives the result $523565$. This is roughly $\frac{1}{16}$ of the number of bytes in the 8192KB of process stack space my system provides, and since there are two prints of the word "func" per call to `func()` it seems likely that each recursive call occupies 32 bytes of stack memory.
