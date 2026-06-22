# Chapter 19 — Multi-File Projects, Header Files, and Build Systems

## Introduction

Every program in this tutorial so far has lived in a single `.c` file. Real C projects almost never do. As a codebase grows, a single file becomes slow to recompile, hard to navigate, and impossible to share cleanly between teams or reuse across projects. This chapter covers how C projects are actually organized: splitting code across multiple files, the role of header files, how to compile and link multi-file projects with `gcc`, sharing global state across files with `extern`, and automating the whole process with a Makefile. It closes with a short conceptual look at static and dynamic libraries, which are the natural next step once a project's reusable pieces are mature enough to be shared without source code.

## Why Split Code Across Multiple Files

Four practical reasons drive splitting a program into multiple files:

- **Organization.** Related functions and types live together (e.g. all math helpers in one file, all string helpers in another), making the codebase easier to navigate.
- **Faster incremental recompilation.** If only one file changes, only that file needs to be recompiled and re-linked — not the entire program. On a large project this is the difference between a multi-minute rebuild and a near-instant one.
- **Reuse.** A well-isolated file of utility functions can be dropped into a different project with minimal changes.
- **Separate compilation.** C was designed so that files can be compiled independently into object files and linked together afterward, without any file needing to see another file's source code — only its declarations.

## Header Files vs. Source Files

A C project's files split into two roles:

- **Header files (`.h`)** contain *declarations*: function prototypes, struct/type definitions, `#define` macros, and `extern` variable declarations. A header tells other files *what exists and how to use it*, without supplying the implementation.
- **Source files (`.c`)** contain *definitions*: the actual implementation of functions, and the actual storage for global variables.

A source file that wants to use code from another source file `#include`s that file's header — never the other file's `.c` file directly.

Recall the include guard pattern from Chapter 15: every header must be wrapped in `#ifndef`/`#define`/`#endif` (or use `#pragma once` on supporting compilers) so that accidentally including the same header twice in one translation unit does not cause duplicate-definition errors.

```c
#ifndef MATHUTILS_H
#define MATHUTILS_H

/* declarations go here */

#endif /* MATHUTILS_H */
```

## A Complete Worked Multi-File Example

The following three files together form one program: a header declaring two math functions, a source file implementing them, and a `main.c` that uses them.

`mathutils.h`:

```c
#ifndef MATHUTILS_H
#define MATHUTILS_H

int add(int a, int b);
int gcd(int a, int b);

#endif /* MATHUTILS_H */
```

`mathutils.c`:

```c
#include "mathutils.h"

int add(int a, int b) {
    return a + b;
}

int gcd(int a, int b) {
    while (b != 0) {
        int temp = b;
        b = a % b;
        a = temp;
    }
    return a;
}
```

`main.c`:

```c
#include <stdio.h>
#include "mathutils.h"

int main(void) {
    int x = 12, y = 18;
    printf("add(%d, %d) = %d\n", x, y, add(x, y));
    printf("gcd(%d, %d) = %d\n", x, y, gcd(x, y));
    return 0;
}
```

Note the quotes versus angle brackets: `#include "mathutils.h"` (quotes) tells the preprocessor to look for the header in the current project directory first, while `#include <stdio.h>` (angle brackets) tells it to look in the standard system include paths.

## Compiling Multi-File Projects with gcc

### Method 1: compile and link in one step

```bash
gcc -std=c11 -Wall -Wextra main.c mathutils.c -o app
./app
```

### Expected output

```text
add(12, 18) = 30
gcd(12, 18) = 6
```

This single command compiles both `.c` files and links them into the executable `app` in one pass. It is simple and fine for small projects, but it always recompiles *everything*, even files that have not changed since the last build.

### Method 2: separate compilation, then linking

```bash
gcc -std=c11 -Wall -Wextra -c mathutils.c -o mathutils.o
gcc -std=c11 -Wall -Wextra -c main.c -o main.o
gcc main.o mathutils.o -o app
./app
```

### Expected output

```text
add(12, 18) = 30
gcd(12, 18) = 6
```

The `-c` flag tells `gcc` to compile a source file into an **object file** (`.o`) without linking — an intermediate, not-yet-executable form. The final `gcc main.o mathutils.o -o app` command links the object files together into the executable.

The payoff for this extra step: if you later change only `main.c`, you only need to re-run `gcc -c main.c -o main.o` and re-link — `mathutils.c` does not need to be touched at all. On a project with dozens or hundreds of source files, only recompiling what changed (instead of everything, every time) is the difference between a build that takes seconds and one that takes many minutes. This is precisely the mechanism a Makefile automates.

## Sharing a Global Variable Across Files with `extern`

Chapter 16 briefly introduced `extern` as a way to declare that a variable's actual storage lives elsewhere. Here is the complete pattern, worked through fully: declare the variable as `extern` in a shared header, define it (give it real storage) in exactly one `.c` file, and use it freely from any other file that includes the header.

`counter.h`:

```c
#ifndef COUNTER_H
#define COUNTER_H

extern int counter;          /* declaration: "this variable exists somewhere" */

void increment_counter(void);

#endif /* COUNTER_H */
```

`counter.c`:

```c
#include "counter.h"

int counter = 0;             /* definition: this is where it actually lives */

void increment_counter(void) {
    counter++;
}
```

`extern_main.c`:

```c
#include <stdio.h>
#include "counter.h"

int main(void) {
    printf("counter starts at %d\n", counter);
    increment_counter();
    increment_counter();
    increment_counter();
    printf("counter is now %d\n", counter);
    return 0;
}
```

### Compile and run

```bash
gcc -std=c11 -Wall -Wextra extern_main.c counter.c -o extern_demo
./extern_demo
```

### Expected output

```text
counter starts at 0
counter is now 3
```

`extern_main.c` never defines `counter` — it only sees the `extern int counter;` declaration via `counter.h`, which promises the linker that storage for `counter` exists *somewhere* in the final program. `counter.c` is the one file that actually defines it (`int counter = 0;`, with no `extern` keyword, allocating real storage). If no file defines it, linking fails with an "undefined reference" error; if more than one `.c` file defines it (without `extern`), linking fails with a "multiple definition" error.

!!! warning "Global variables are usually a last resort"
    `extern` globals are shown here because they are a real, commonly encountered pattern, but reaching for shared mutable global state should be a deliberate, occasional choice, not a habit. Most of the time, passing data explicitly through function parameters (as in every previous chapter) produces code that is easier to reason about and test.

## Writing a Makefile

Typing out multi-step compile commands by hand does not scale. `make` is a build-automation tool that reads a file named `Makefile` describing **targets**, their **dependencies**, and the **recipe** (shell commands) used to build them. `make` only rebuilds a target if it is missing or older than one of its dependencies — this is what gives incremental builds their speed.

A basic Makefile rule looks like:

```makefile
target: dependency1 dependency2
	command to build target
```

The command line(s) under a rule **must** be indented with a literal tab character, not spaces — this is a long-standing and frequently tripped-over quirk of `make`'s syntax.

Key pieces:

- **Variables** like `CC` (the compiler to invoke) and `CFLAGS` (compiler flags) are conventional names that make a Makefile easy to adjust in one place.
- **Automatic variables**: `$@` expands to the current target's name; `$^` expands to the full list of that target's dependencies. Using these instead of repeating names by hand keeps a Makefile correct when files are renamed.
- A **phony target** like `clean` does not correspond to an actual file being built. Declaring it with `.PHONY: clean` tells `make` not to get confused if a file literally named `clean` ever exists in the directory, and that this target should always run when invoked.

Here is a complete, working Makefile for the `mathutils`/`main` example:

```makefile
CC = gcc
CFLAGS = -std=c11 -Wall -Wextra -g

TARGET = app
OBJS = main.o mathutils.o

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) $^ -o $@

main.o: main.c mathutils.h
	$(CC) $(CFLAGS) -c main.c -o $@

mathutils.o: mathutils.c mathutils.h
	$(CC) $(CFLAGS) -c mathutils.c -o $@

clean:
	rm -f $(TARGET) $(OBJS)

.PHONY: clean
```

### Build and run

```bash
make
./app
```

### Expected output

```text
gcc -std=c11 -Wall -Wextra -g -c main.c -o main.o
gcc -std=c11 -Wall -Wextra -g -c mathutils.c -o mathutils.o
gcc -std=c11 -Wall -Wextra -g main.o mathutils.o -o app
add(12, 18) = 30
gcd(12, 18) = 6
```

### Clean up build artifacts

```bash
make clean
```

This removes `app`, `main.o`, and `mathutils.o`, leaving only source files behind. Running `make` again afterward rebuilds everything from scratch, since the object files and executable no longer exist.

Try editing only `mathutils.c` (e.g. adding a comment) and running `make` again: `make` recompiles `mathutils.o` and re-links `app`, but does **not** recompile `main.c`, because `main.o` is still newer than its dependencies (`main.c` and `mathutils.h`). This selective rebuilding is exactly the incremental-compilation benefit described earlier in this chapter, now automated.

!!! tip "Listing headers as dependencies"
    Notice that both `main.o` and `mathutils.o` list `mathutils.h` as a dependency, even though neither rule's command mentions the header directly. This is intentional: if `mathutils.h` changes (say, a function's parameters change), both object files that include it need to be recompiled, and listing it as a dependency is what tells `make` that.

## Static vs. Dynamic Libraries: A Conceptual Closing Note

Once a set of `.c`/`.h` files matures into something genuinely reusable across multiple projects, the next step beyond "recompile the source every time" is packaging it as a **library** — a precompiled bundle of object code that other programs link against without needing your original source.

There are two kinds:

- **Static libraries** (`.a` on Linux/macOS, `.lib` on Windows) are archives of object files that get copied directly into the final executable at link time. The resulting binary is self-contained and has no runtime dependency on the library being present on the target machine, at the cost of a larger executable, and every program statically linking the same library carries its own separate copy.
- **Dynamic (shared) libraries** (`.so` on Linux, `.dll` on Windows, `.dylib` on macOS) are *not* copied into the executable. Instead, the executable records that it needs the library, and the operating system loads and links it in at the moment the program actually runs. This keeps individual executables smaller and allows many running programs to share a single copy of the library in memory, but it means the shared library file must actually be present (and compatible) on whatever machine runs the program — a missing or mismatched shared library at runtime is a common source of "it works on my machine" deployment problems.

Choosing between them is a tradeoff between distribution simplicity (static: just ship the one binary) and efficiency/footprint (dynamic: smaller binaries, shared memory usage, easier to patch the library without recompiling every program that uses it). Building an actual `.a` or `.so` is outside this tutorial's scope, but recognizing the terms — and the static/dynamic tradeoff — is enough to navigate most real-world C build systems and dependency documentation you will encounter going forward.

## Summary

- Splitting a program across multiple files improves organization, lets `make` (or any build tool) recompile only what changed, and enables reuse and true separate compilation.
- Header files (`.h`) declare; source files (`.c`) define. Every header needs an include guard, as covered in Chapter 15.
- `gcc main.c mathutils.c -o app` compiles and links in one step; `gcc -c file.c -o file.o` followed by `gcc *.o -o app` compiles separately and then links, enabling incremental rebuilds.
- `extern` in a shared header declares that a global variable's storage lives in exactly one `.c` file, which defines it without the `extern` keyword; every other file that includes the header can then use the same shared variable.
- A Makefile encodes targets, dependencies, and recipes; `make` rebuilds a target only when it is missing or older than its dependencies. `$@` and `$^` are automatic variables for the target's name and its dependency list; `.PHONY` marks targets like `clean` that do not correspond to real files.
- Static libraries (`.a`) are linked directly into an executable (self-contained, larger); dynamic libraries (`.so`/`.dll`) are loaded at runtime (smaller binaries, shared across programs, require the library to be present on the target machine).

## Exercises

1. Take the `mathutils` example and add a third function, `int is_prime(int n)`, declared in `mathutils.h` and defined in `mathutils.c`. Update `main.c` to use it, and rebuild using both the one-step and the separate-compilation methods.
2. Extend the provided Makefile to also build a second executable from a different `main2.c` that reuses the same `mathutils.o`, without recompiling `mathutils.c` if it has not changed.
3. Deliberately remove the include guard from `mathutils.h`, have `main.c` include it twice in a row, and observe and explain the resulting compiler error.
4. Create a header `stringutils.h` and `stringutils.c` exposing a `char *reverse_string(const char *input)` function (allocated with `malloc`, recalling Chapter 13's ownership rules — document in a comment whose responsibility it is to `free` the result), and use it from a new `main.c`.
5. *(Harder)* Write a Makefile with a pattern rule (`%.o: %.c`) that automatically compiles any `.c` file into a matching `.o` file, instead of writing one explicit rule per file, and verify it still produces the same `app` binary as the hand-written version in this chapter.
6. *(Harder)* Research (or experiment with) building `mathutils.c` into an actual static library (`ar rcs libmathutils.a mathutils.o`) and linking `main.c` against it with `-lmathutils -L.`; write down the exact commands you used and the resulting file sizes compared to the non-library build.

---
[← Chapter 18 — Data Structures in C](chapter-18.md) | [Chapter 20 — Debugging and Best Practices →](chapter-20.md)
