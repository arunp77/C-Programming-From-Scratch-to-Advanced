# Chapter 20 — Debugging and Best Practices

## Introduction

Writing correct C is not just about knowing the language's rules — it is about knowing how to find and fix the inevitable mistakes, and about writing code in a style that prevents whole categories of mistakes from happening in the first place. This closing chapter covers the practical side of professional C development: classifying the kinds of bugs you will encounter, using the compiler itself as a bug-finding tool, debugging interactively with `gdb`, the enduring usefulness of `printf` debugging, finding memory errors with Valgrind, and a checklist of defensive habits drawn from everything covered in this tutorial.

## Categories of Bugs

Not all bugs are the same kind of problem, and recognizing which kind you are facing changes how you look for it.

- **Compile-time syntax errors.** The compiler refuses to produce a program at all — a missing semicolon, mismatched braces, an undeclared identifier. These are the easiest category: the compiler tells you exactly where and (usually) why.
- **Logic errors.** The program compiles and runs without crashing, but produces the wrong answer — an off-by-one loop bound, a `<` where you meant `<=`, a misremembered formula. The compiler cannot help here; only careful reading, testing, and debugging tools can.
- **Runtime errors.** The program crashes while running — typically a segmentation fault from dereferencing a bad pointer, an array index far out of bounds, or a stack overflow from unbounded recursion (Chapter 16). These announce themselves loudly, but not always *where* the real mistake was made.
- **Undefined behavior.** The C standard explicitly does not define what happens for certain operations — the program might crash, might silently produce wrong results, or might "work" perfectly today and break the moment you change compilers, optimization flags, or hardware. This is the most dangerous category because it gives no reliable signal that anything is wrong.

Several examples of undefined behavior have already appeared in earlier chapters, even though the term may not have been used explicitly at the time:

- Reading or writing past the end of an array (Chapter 9) — the index is out of bounds, but nothing stops the read/write from happening; it simply touches memory that does not belong to that array.
- Dereferencing a dangling pointer (Chapter 13) — a pointer left pointing at memory that has already been `free`d.
- Using an uninitialized variable's value before it has been assigned (covered in this chapter, below) — the variable holds whatever bits happened to already be in that memory.
- Signed integer overflow (briefly touched on in earlier arithmetic discussions) — unlike unsigned overflow, which wraps around predictably, signed overflow's behavior is not defined by the standard at all.

!!! danger "Undefined behavior is not 'probably fine'"
    A program that exhibits undefined behavior but happens to print the expected output is not a correct program — it is a program that got lucky on this run, on this compiler, with these flags. Treat any code path you know relies on undefined behavior as a bug to fix, regardless of whether it currently "seems to work."

## Using Compiler Warnings Aggressively

The compiler can catch a surprising number of logic errors and undefined-behavior risks before the program ever runs, if you ask it to look. The flags `-Wall -Wextra -Wpedantic` enable broad warning categories; `-Werror` turns every warning into a hard compile error.

```bash
gcc -std=c11 -Wall -Wextra -Wpedantic -Werror program.c -o program
```

Consider this small, deliberately buggy program:

```c
#include <stdio.h>

int sum_array(int *array, int length) {
    int total;  /* BUG: never initialized */
    for (int i = 0; i < length; i++) {
        total += array[i];
    }
    return total;
}

int main(void) {
    int numbers[] = { 1, 2, 3, 4, 5 };
    int length = 5;
    int result = sum_array(numbers, length);
    printf("Sum is %d\n", result);
    return 0;
}
```

`total` is never initialized to `0` before the loop accumulates into it, so its starting value is whatever garbage bits already happened to occupy that stack slot — using it is undefined behavior, and the printed sum is unreliable. Compiling with optimization enabled lets the compiler's data-flow analysis catch this:

```bash
gcc -std=c11 -Wall -Wextra -O2 -g uninitialized.c -o uninitialized
```

### Expected compiler output

```text
uninitialized.c: In function 'sum_array':
uninitialized.c:6:15: warning: 'total' is used uninitialized [-Wuninitialized]
    6 |         total += array[i];
      |         ~~~~~~^~~~~~~~~~~
uninitialized.c:4:9: note: 'total' was declared here
    4 |     int total;  /* BUG: never initialized */
      |         ^~~~~
```

Adding `-Werror` to that command would make this warning a compile failure instead of a warning that is easy to scroll past — exactly the point. On a real project, treating every warning as a hard stop means bugs like this one get fixed the moment they are introduced, not weeks later when they cause a confusing crash or wrong output in production. The fix here is simply `int total = 0;`.

!!! tip "Why -Werror is worth the friction"
    `-Werror` feels strict at first, especially while learning. On any project beyond a quick experiment, it pays for itself many times over: a warning ignored today is a bug report tomorrow, and "the build doesn't compile" is a far louder, far earlier signal than "the program is subtly wrong in production."

## Debugging with gdb

`gdb` (the GNU Debugger) lets you pause a running program, inspect its variables, and step through its execution line by line — invaluable for logic errors and crashes that are hard to diagnose just by reading the source.

Using the same `sum_array` example from above (with the uninitialized-`total` bug left in place), first compile with debug information using `-g`:

```bash
gcc -std=c11 -g uninitialized.c -o uninitialized
```

The `-g` flag embeds source-level debug information (variable names, line numbers, type info) into the binary, without which `gdb` could only show raw addresses and machine instructions.

Start `gdb` on the binary:

```bash
gdb ./uninitialized
```

A typical session, with commands you type shown after the `(gdb)` prompt:

```text
(gdb) break sum_array
Breakpoint 1 at 0x1178: file uninitialized.c, line 5.
(gdb) run
Starting program: /path/to/uninitialized

Breakpoint 1, sum_array (array=0x7fffffffd940, length=5) at uninitialized.c:5
5           for (int i = 0; i < length; i++) {
(gdb) next
6               total += array[i];
(gdb) next
5           for (int i = 0; i < length; i++) {
(gdb) print total
$1 = 1
(gdb) print i
$2 = 0
(gdb) backtrace
#0  sum_array (array=0x7fffffffd940, length=5) at uninitialized.c:5
#1  0x0000555555555201 in main () at uninitialized.c:14
(gdb) continue
Continuing.
Sum is 15
[Inferior 1 (process 241145) exited normally]
(gdb)
```

Walking through each command:

- `break sum_array` sets a **breakpoint**: execution will pause as soon as `sum_array` is entered. `break <line-number>` works the same way for a specific line instead of a function name.
- `run` starts the program; it executes normally until it hits a breakpoint.
- `next` executes the current line and stops at the following one, stepping *over* function calls. `step` behaves the same way except it steps *into* a function call if the current line makes one — useful when you need to see what happens inside a called function.
- `print <expression>` evaluates and prints a variable or expression in the current scope — here, `print total` reveals that `total` already holds the value `1` after only one loop iteration, even though it was never explicitly initialized, confirming the bug.
- `backtrace` (or `bt`) prints the current **call stack**: every function call still active, from the innermost (`sum_array`) outward to `main`. Recall from Chapter 16 that this is the same call stack recursion builds up — `backtrace` is the tool that lets you actually see it at any paused moment.
- `continue` resumes normal execution until the program finishes or hits another breakpoint.

For a crash (a runtime error such as a segmentation fault), the workflow is similar but you typically skip setting a breakpoint and just `run` the program; `gdb` automatically pauses at the moment of the crash, and an immediate `backtrace` usually points straight at the offending line.

## printf Debugging

Despite `gdb`'s power, simply inserting `printf` calls to print variable values at key points remains a fast, legitimate debugging technique — especially for quickly narrowing down *where* in a long-running or loop-heavy program something goes wrong, before reaching for a full interactive debugging session.

```c
#include <stdio.h>

int sum_array(int *array, int length) {
    int total = 0;
    for (int i = 0; i < length; i++) {
        total += array[i];
        printf("DEBUG: i=%d array[i]=%d total=%d\n", i, array[i], total);
    }
    return total;
}

int main(void) {
    int numbers[] = { 1, 2, 3, 4, 5 };
    int result = sum_array(numbers, 5);
    printf("Sum is %d\n", result);
    return 0;
}
```

### Compile and run

```bash
gcc -std=c11 -Wall -Wextra printf_debug.c -o printf_debug
./printf_debug
```

### Expected output

```text
DEBUG: i=0 array[i]=1 total=1
DEBUG: i=1 array[i]=2 total=3
DEBUG: i=2 array[i]=3 total=6
DEBUG: i=3 array[i]=4 total=10
DEBUG: i=4 array[i]=5 total=15
Sum is 15
```

The downside of scattering raw `printf` debug statements through a codebase is forgetting to remove them, leaving debug noise in production output. Chapter 15 introduced the `#ifdef DEBUG` pattern for exactly this situation — wrap debug prints so they compile away entirely unless a `DEBUG` macro is defined:

```c
#include <stdio.h>

#ifdef DEBUG
#define DEBUG_PRINT(...) fprintf(stderr, __VA_ARGS__)
#else
#define DEBUG_PRINT(...) /* expands to nothing in a release build */
#endif

int sum_array(int *array, int length) {
    int total = 0;
    for (int i = 0; i < length; i++) {
        total += array[i];
        DEBUG_PRINT("i=%d array[i]=%d total=%d\n", i, array[i], total);
    }
    return total;
}

int main(void) {
    int numbers[] = { 1, 2, 3, 4, 5 };
    int result = sum_array(numbers, 5);
    printf("Sum is %d\n", result);
    return 0;
}
```

### Compile and run (debug prints disabled)

```bash
gcc -std=c11 -Wall -Wextra debug_macro.c -o debug_macro
./debug_macro
```

### Compile and run (debug prints enabled)

```bash
gcc -std=c11 -Wall -Wextra -DDEBUG debug_macro.c -o debug_macro_verbose
./debug_macro_verbose
```

With `-DDEBUG` passed on the command line, `DEBUG_PRINT` expands to a real `fprintf` call to `stderr`; without it, the macro expands to nothing, and the debug statements cost nothing in the shipped build — not even a function call.

## Valgrind for Memory Errors

Chapter 13 mentioned that tools exist for catching memory leaks and other heap mistakes automatically. Valgrind is the standard one on Linux. It runs your *already-compiled* program inside an instrumented virtual environment that tracks every allocation, every free, and every memory access, then reports anything suspicious when the program exits.

Consider a small program with two classic mistakes: a buffer overrun and a leaked allocation.

```c
#include <stdlib.h>

int main(void) {
    int *array = malloc(5 * sizeof(int));
    if (array == NULL) {
        return 1;
    }

    for (int i = 0; i <= 5; i++) {   /* BUG: <= instead of <, writes one past the end */
        array[i] = i;
    }

    /* BUG: array is never freed before the program exits */
    return 0;
}
```

Compile with `-g` so Valgrind's output can reference real source lines:

```bash
gcc -std=c11 -Wall -Wextra -g memory_bug.c -o memory_bug
```

Run it under Valgrind's leak checker:

```bash
valgrind --leak-check=full ./memory_bug
```

### Representative Valgrind output

```text
==12345== Memcheck, a memory error detector
==12345== Command: ./memory_bug
==12345==
==12345== Invalid write of size 4
==12345==    at 0x10918A: main (memory_bug.c:10)
==12345==  Address 0x4a4b074 is 0 bytes after a block of size 20 alloc'd
==12345==    at 0x484880F: malloc (vg_replace_malloc.c:446)
==12345==    by 0x109172: main (memory_bug.c:4)
==12345==
==12345== HEAP SUMMARY:
==12345==     in use at exit: 20 bytes in 1 blocks
==12345==   total heap usage: 1 allocs, 0 frees, 20 bytes allocated
==12345==
==12345== 20 bytes in 1 blocks are definitely lost in loss record 1 of 1
==12345==    at 0x484880F: malloc (vg_replace_malloc.c:446)
==12345==    by 0x109172: main (memory_bug.c:4)
==12345==
==12345== LEAK SUMMARY:
==12345==    definitely lost: 20 bytes in 1 blocks
==12345==    indirectly lost: 0 bytes in 0 blocks
==12345==      possibly lost: 0 bytes in 0 blocks
==12345==    still reachable: 0 bytes in 0 blocks
==12345==         suppressed: 0 bytes in 0 blocks
```

Reading this practically, top to bottom:

- **"Invalid write of size 4"** with **"0 bytes after a block of size 20"** is the buffer overrun: Valgrind caught the write to `array[5]`, immediately past the 20-byte (5-`int`) block, and points directly at `memory_bug.c:10` — the loop — as well as line 4, where that block was originally allocated.
- **"HEAP SUMMARY"** reports that the program performed one allocation and zero frees before exiting — an immediate red flag.
- **"definitely lost: 20 bytes in 1 blocks"** is Valgrind's leak classification for memory that was allocated, never freed, and is now unreachable — there is no remaining pointer anywhere that could free it. This is the most actionable leak category. (Valgrind also reports "indirectly lost," "possibly lost," and "still reachable" categories for less clear-cut situations; "definitely lost" is the one to always treat as a bug.)

The fix is the same discipline Chapter 13 already covered: change the loop bound to `i < 5`, and add `free(array);` before `return 0;`.

!!! note "Valgrind availability"
    Valgrind is a Linux-native tool (with limited macOS support on older Intel Macs) and is not part of a default gcc installation — install it via your distribution's package manager (e.g. `sudo apt install valgrind` on Debian/Ubuntu). On Windows, tools like Dr. Memory or running inside WSL serve a similar purpose.

## Defensive Coding Habits Checklist

The following habits, drawn together from across this entire tutorial, separate code that merely "works on the example I tried" from code that holds up under real use:

- **Always check return values.** `malloc` and `calloc` can return `NULL`; `fopen` can return `NULL`; `scanf` returns the number of items it successfully read. Checking these every time (Chapters 13, 14, 5) is what turns "crashes mysteriously later" into "fails immediately and clearly, right where the problem actually is."
- **Always initialize variables.** An uninitialized local variable holds indeterminate bits, not zero — as demonstrated earlier in this very chapter. Initialize at declaration whenever a sensible default exists.
- **Avoid magic numbers.** A bare `7` or `100` scattered through code says nothing about what it means or why it was chosen. Named constants (`#define MAX_STUDENTS 100`, or an `enum`, both from Chapter 15) document intent and make changing the value later a one-line edit instead of a search-and-hope.
- **Use consistent naming and formatting.** A single, consistently applied style (indentation, brace placement, naming convention for functions/variables/constants) reduces the mental overhead of reading code, including your own code from a few weeks ago.
- **Keep functions small and focused.** A function that does one clearly nameable thing is easier to test, easier to reuse, and easier to verify by reading. If a function's name needs "and" to describe it, it is usually a sign it should be split in two.
- **Comment why, not what.** `i++; /* increment i */` tells a reader nothing they couldn't already see. A comment earns its place by explaining a reason that is not visible in the code itself — why this particular approach, why this edge case needs special handling, why a value is computed this way instead of some more obvious way.

## Closing Recap

Across this tutorial, a small set of habits has been emphasized repeatedly because they are what actually prevents the bug categories from this chapter:

- Check everything that can fail (`malloc`, `fopen`, `scanf`) before trusting its result.
- Pair every `malloc` with a matching `free`, and never use a pointer after the memory it points to has been freed.
- Compile with `-Wall -Wextra -Wpedantic` (and `-Werror` on serious projects) from the very first line of a new project, not bolted on afterward.
- Reach for `gdb` when you need to see exactly what a program is doing at a precise moment; reach for `printf`/`DEBUG_PRINT` when you need a fast, broad picture across many iterations; reach for Valgrind whenever memory correctness itself is in question.
- Prefer clear, small, well-named functions and named constants over clever, dense, or magic-number-laden code — future readers (including future you) will be debugging it, not admiring it.

These habits, more than any single language feature, are what separate code that compiles from code that can actually be trusted.

## Summary

- Bugs fall into four categories: compile-time syntax errors (compiler refuses to build), logic errors (wrong behavior, compiles fine), runtime errors (crashes), and undefined behavior (unreliable, unspecified behavior that may appear to work by accident).
- `-Wall -Wextra -Wpedantic -Werror` turn the compiler into an active bug-finding tool; `-Werror` converts warnings into hard failures, which is good discipline for any serious project.
- `gdb` (compiled with `-g`) supports breakpoints (`break`), running (`run`), stepping (`next`/`step`), inspecting state (`print`), and viewing the call stack (`backtrace`) — essential for runtime errors and hard-to-trace logic errors.
- `printf` debugging remains fast and legitimate for broad, iteration-heavy diagnosis; guard debug prints with the `#ifdef DEBUG` pattern (Chapter 15) so they compile away cleanly in release builds.
- Valgrind's `--leak-check=full` instruments a compiled program to catch invalid memory accesses and classify leaked memory (with "definitely lost" being the category to always fix), directly extending the leak/dangling-pointer concerns first raised in Chapter 13.
- Defensive habits — checking return values, initializing variables, naming constants instead of using magic numbers, consistent style, small focused functions, and comments that explain why — are what make code trustworthy beyond the first example you tried.

## Exercises

1. Take any program from an earlier chapter that calls `malloc` without checking its return value, and add a proper `NULL` check that prints an error message and exits cleanly.
2. Compile a program of your own with `-Wall -Wextra -Wpedantic` and fix every warning it produces, even ones that "don't seem to matter" — note in a comment what each one was actually warning you about.
3. Take the `memory_bug.c` example from this chapter, fix both bugs, and re-run it under Valgrind to confirm the "definitely lost" report and the invalid-write report both disappear.
4. Write a program with a deliberate segmentation fault (e.g. dereferencing a `NULL` pointer), compile it with `-g`, and use `gdb`'s `run` followed by `backtrace` to locate the exact line that crashed.
5. *(Harder)* Wrap a small set of debug `printf` calls in your own `DEBUG_PRINT` macro guarded by `#ifdef DEBUG`, then write a Makefile (Chapter 19) with two targets — `debug` (passes `-DDEBUG`) and `release` (does not) — that build two versions of the same program.
6. *(Harder)* Pick a program you wrote in an earlier chapter that uses several bare numeric literals; refactor it to replace every magic number with a named constant or `enum`, and write a short comment explaining what changed and why it is clearer now.

---
[← Chapter 19 — Multi-File Projects, Header Files, and Build Systems](chapter-19.md) | [Appendix A — C Standard Library Reference →](Appendix-A.md)
