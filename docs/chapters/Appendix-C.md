# Appendix C — Practice Projects and Next Steps

Reading a tutorial builds vocabulary; building projects builds fluency. The projects below are ordered roughly from easiest to hardest, and each lists the specific earlier chapters it draws on, so you can pick one that matches what you most want to reinforce.

## Practice Projects

### 1. Command-Line Calculator

A program that reads two numbers and an operator from the user and prints the result, looping until the user chooses to quit. Extend it to support a small expression language if you want an extra challenge (e.g. handling operator precedence between `+`/`-` and `*`/`/`).

- Concepts exercised:
    - Chapter 5 — Input and Output
    - Chapter 6 — Decision Making (if, else, switch)
    - Chapter 7 — Loops and Iteration

### 2. Number Guessing Game

The program picks a random number in a range; the player guesses repeatedly and is told "too high" or "too low" until they guess correctly, with a count of attempts taken at the end.

- Concepts exercised:
    - Chapter 7 — Loops and Iteration
    - Chapter 6 — Decision Making (if, else, switch)
    - `<stdlib.h>`'s `rand`/`srand` and `<time.h>`'s `time` (Appendix A)

### 3. Tic-Tac-Toe (Two-Player, Console)

A 3x3 grid stored in a 2D array, alternating turns between two players who enter row/column coordinates, with win and draw detection after each move.

- Concepts exercised:
    - Chapter 8/9 — Arrays (one- and two-dimensional)
    - Chapter 6 — Decision Making
    - Chapter 7 — Loops and Iteration

### 4. Small Matrix Operations Library

A set of functions operating on 2D arrays (or dynamically allocated grids): matrix addition, multiplication, and transpose, with a small `main` that demonstrates each on sample matrices.

- Concepts exercised:
    - Chapter 9 — Arrays
    - Chapter 17 — Advanced Pointers (the array-of-pointers vs. flat-block tradeoff, if implemented dynamically)
    - Chapter 19 — Multi-File Projects (split into `matrix.h`/`matrix.c`/`main.c`)

### 5. Simple Inventory/CRUD Manager Backed by a Text File

A program that creates, reads, updates, and deletes inventory records (e.g. item name, quantity, price), persisting them to a text file between runs so data survives program restarts.

- Concepts exercised:
    - Chapter 14 — File Handling
    - Chapter 12 — Structs
    - Chapter 13 — Dynamic Memory (if records are loaded into a dynamically sized array)

### 6. Text Adventure Game

A small interactive fiction engine: the player moves between rooms described by text, with simple inventory and basic conditional logic gating progress (e.g. "you need the key to open this door").

- Concepts exercised:
    - Chapter 6 — Decision Making
    - Chapter 12 — Structs (for rooms, items)
    - Chapter 18 — Data Structures in C (a linked list or array of rooms/connections)

### 7. Student Grade Management System Using Structs and File I/O

A program that stores student records (name, ID, a list of scores) in structs, computes averages and letter grades, and saves/loads the full roster to/from a file.

- Concepts exercised:
    - Chapter 12 — Structs
    - Chapter 14 — File Handling
    - Chapter 13 — Dynamic Memory (if the roster size is not fixed at compile time)

### 8. Linked-List-Based To-Do List App

A console to-do list backed by a singly (or doubly) linked list: add a task, mark complete, remove a task, list all tasks, with the list persisted to a file between runs.

- Concepts exercised:
    - Chapter 18 — Data Structures in C (linked lists)
    - Chapter 14 — File Handling
    - Chapter 13 — Dynamic Memory

### 9. Tiny Line-Based Shell/Command Interpreter

A program that reads a line of input, splits it into a command name and arguments, and dispatches to one of several built-in functions (e.g. `echo`, `add`, `list`, `exit`) — a simplified illustration of how real shells parse and execute commands.

- Concepts exercised:
    - Chapter 13/Chapter 9 — String handling and parsing
    - Chapter 17 — Advanced Pointers (a dispatch table of function pointers mapping command names to handler functions)
    - Chapter 7 — Loops and Iteration (the read-eval loop itself)

### 10. Simple Hash Table Implementation

A hash table using either open addressing (linear or quadratic probing) or chaining (a linked list per bucket), supporting insert, lookup, and delete by string key, with a basic hash function (e.g. a simple polynomial rolling hash over the key's characters).

- Concepts exercised:
    - Chapter 18 — Data Structures in C (linked lists, if implementing chaining)
    - Chapter 17 — Advanced Pointers (arrays of pointers/structs for the bucket array)
    - Chapter 13 — Dynamic Memory (growing the table and rehashing as it fills)

## What to Learn Next

Finishing this tutorial means you have a genuinely solid foundation in C — pointers, memory management, data structures, multi-file builds, and debugging discipline. From here, a few directions are particularly natural extensions of what you already know:

- **Data structures and algorithms, more formally.** This tutorial implemented linked lists, stacks, queues, and binary search trees by hand, but stopped short of balancing strategies (AVL/red-black trees), graph algorithms, hashing theory, and complexity analysis in real depth. A dedicated algorithms course or textbook builds directly on Chapter 18's foundation.
- **Operating systems concepts.** Processes, threads, virtual memory, and system calls are the layer directly underneath everything C has been doing all along — `malloc` requesting memory from the OS, file I/O going through system calls, the call stack you explored in Chapter 16 being a real, OS-managed region of memory. Understanding an OS course's material is dramatically easier with C already in hand.
- **Networking and sockets programming in C.** The BSD sockets API (`socket`, `bind`, `listen`, `accept`, `connect`, `send`, `recv`) is itself a C API, and writing a simple TCP client/server is a natural next project that exercises file-descriptor-style I/O, structs (for address information), and careful error checking, all building on this tutorial directly.
- **Contributing to a real-world open-source C project.** Reading a mature, real C codebase (e.g. a Unix utility, SQLite, Redis, a piece of `git`) and tracing how it uses the exact tools from this tutorial — headers, Makefiles, function pointers, careful memory ownership — is one of the fastest ways to deepen fluency past what any tutorial alone can teach.
- **Moving on to C++ or Rust.** With C's manual memory management and pointer model genuinely understood, both C++ (which adds object-oriented features, templates, and RAII-based automatic resource management on top of a C-like core) and Rust (which enforces memory safety at compile time through its ownership and borrowing system, solving the exact dangling-pointer and leak problems from Chapter 13 with compiler-checked guarantees) become much more approachable — both languages' design decisions make far more sense once you have felt, firsthand, the problems they exist to solve.

There is no single "correct" next step — pick whichever direction matches what made you want to learn C in the first place, and keep building things.

---
[← Appendix B — Cheat Sheets](Appendix-B.md)
