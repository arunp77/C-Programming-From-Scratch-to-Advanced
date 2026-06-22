# Chapter 15 — Preprocessor Directives and Macros

## Introduction

Chapter 2 briefly mentioned that a C program passes through a **preprocessing** step before real compilation begins, and Chapter 3 briefly introduced `#define` for naming constants. This chapter returns to the preprocessor in full: what it actually does, the full set of directives it understands, the sharp edges of macros that look like functions but are not, and the conditional-compilation tools that let a single source file produce different compiled programs depending on settings chosen at build time.

## The Preprocessor Runs Before Compilation, on Text

The **preprocessor** is a distinct, earlier pass over your source code, run automatically by `gcc` before the actual C compiler ever sees the result. It does not understand C syntax, types, or semantics at all — it operates purely on **text**, following simple textual substitution and inclusion rules. Every line beginning with `#` is a preprocessor directive, an instruction to the preprocessor rather than to the compiler. By the time the compiler runs, every directive has already been resolved, and the compiler sees only plain C — no `#define`s, no `#include`s, just the expanded, assembled text.

You can see this expanded text yourself with `gcc -E`, which runs only the preprocessing step and prints the result:

```bash
gcc -E myprogram.c
```

## `#include`: Bringing in Other Files

`#include` is a directive that textually inserts the entire contents of another file at that exact point, before compilation proceeds.

```c
#include <stdio.h>
#include "myheader.h"
```

- **Angle brackets** `<...>` are used for **system headers** — headers that come with the compiler or standard library, such as `<stdio.h>` or `<stdlib.h>`. The preprocessor searches a set of standard system directories for these.
- **Double quotes** `"..."` are used for **local or project headers** — files you write yourself as part of your own project. The search for these begins in the current file's own directory (and then, depending on configuration, may also fall back to the system directories).

Writing and organizing your own header files — what should go in one, and how multiple `.c` files share them — is covered fully in Chapter 19; for now it is enough to recognize the two include forms and know which one to reach for.

## `#define` Object-Like Macros

Chapter 3 briefly used `#define` to name simple constants:

```c
#define MAX_STUDENTS 100
```

This is called an **object-like macro**: wherever the preprocessor sees the token `MAX_STUDENTS` in the rest of the file, it textually replaces it with `100`, before the compiler ever looks at the code. The substitution is purely mechanical text replacement — the preprocessor has no concept of `MAX_STUDENTS` being an "integer" or anything else; it is simply pattern-and-replace.

```c
#include <stdio.h>

#define MAX_STUDENTS 100
#define PI 3.14159

int main(void) {
    printf("Max students: %d\n", MAX_STUDENTS);
    printf("Pi: %f\n", PI);
    return 0;
}
```

### Compile and run

```bash
gcc -std=c11 -Wall -Wextra definedemo.c -o definedemo
./definedemo
```

### Expected output

```text
Max students: 100
Pi: 3.141590
```

### `#define` vs. `const`: Which to Prefer

`const` (recall Chapter 3) declares a genuine, typed variable that the compiler checks and tracks like any other variable — it occupies memory, has a definite type, and the compiler can report a clear, specific error if you misuse it (e.g., pass it where an incompatible type is expected). A `#define` constant has none of this: it is replaced with raw text before the compiler ever assigns it a type, so a mistake in a macro often produces a confusing error message pointing at the *expanded* code rather than at your original, readable line — or worse, no error at all, just silently wrong behavior.

```c
const int max_students = 100;   /* typed, checked, debuggable */
#define MAX_STUDENTS 100        /* untyped text substitution  */
```

!!! tip "Prefer const or enum over #define for constants in modern C"
    In modern C, `const` (for a single typed constant) or `enum` (for a related group of integer constants, recall Chapter 12) is generally preferable to `#define` for naming constant values, precisely because both give the compiler real type information and produce clearer error messages. `#define` remains useful for things `const` cannot express — conditional compilation (covered later in this chapter) and function-like macros — but for plain constants, reach for `const`/`enum` first.

## Function-Like Macros

A macro can also take parameters, making it look syntactically like a function call:

```c
#define SQUARE(x) ((x) * (x))
```

```c
#include <stdio.h>

#define SQUARE(x) ((x) * (x))

int main(void) {
    int n = 5;
    printf("SQUARE(%d) = %d\n", n, SQUARE(n));
    printf("SQUARE(2 + 3) = %d\n", SQUARE(2 + 3));
    return 0;
}
```

### Compile and run

```bash
gcc -std=c11 -Wall -Wextra squaregood.c -o squaregood
./squaregood
```

### Expected output

```text
SQUARE(5) = 25
SQUARE(2 + 3) = 25
```

This works correctly because of the parentheses around `x` and around the entire expression. To see exactly why those parentheses matter, look at what goes wrong without them.

### Pitfall 1: Missing Parentheses

```c
#include <stdio.h>

#define SQUARE(x) x * x

int main(void) {
    printf("SQUARE(2 + 3) = %d\n", SQUARE(2 + 3));
    return 0;
}
```

### Compile and run

```bash
gcc -std=c11 -Wall -Wextra squarebad.c -o squarebad
./squarebad
```

### Expected output

```text
SQUARE(2 + 3) = 11
```

The expected mathematical answer is `25` (since `2 + 3 = 5` and `5 * 5 = 25`), but the program prints `11`. The reason becomes clear once you remember that the preprocessor does pure text substitution: `SQUARE(2 + 3)` expands literally to `2 + 3 * 2 + 3`, and by ordinary operator precedence (recall Chapter 4) multiplication happens before addition, giving `2 + 6 + 3 = 11`. The fully-parenthesized version, `((x) * (x))`, expands instead to `((2 + 3) * (2 + 3))`, which correctly evaluates to `25`. This is exactly why the working version above wraps both the parameter and the whole expression in parentheses.

### Pitfall 2: Side-Effecting Arguments

Even with correct parentheses, a different problem appears if the argument itself has a **side effect** — a change in program state, such as incrementing a variable:

```c
#include <stdio.h>

#define SQUARE(x) ((x) * (x))

int main(void) {
    int i = 5;
    int result = SQUARE(i++);

    printf("result = %d\n", result);
    printf("i = %d\n", i);

    return 0;
}
```

### Compile and run

```bash
gcc -std=c11 -Wall -Wextra squareside.c -o squareside
./squareside
```

### Expected output

```text
result = 30
i = 7
```

A real function would evaluate `i++` exactly once, pass that single resulting value in, and increment `i` exactly once. But because the macro is pure text substitution, `SQUARE(i++)` expands to `((i++) * (i++))` — `i++` is written out *twice* in the expanded code, so it is evaluated twice: `i` increments from 5 to 6 for the first occurrence, then from 6 to 7 for the second, and the multiplication actually computed is `5 * 6 = 30` rather than the intended `5 * 5 = 25`, while `i` ends up incremented twice instead of once.

!!! warning "Macros are not real functions"
    A function-like macro provides no type checking on its arguments, and — as both pitfalls above show — its arguments can be evaluated zero, one, or multiple times depending entirely on how the macro body uses them, with no warning at the call site. Real functions evaluate each argument exactly once and provide full type checking, which is why a regular function (or, in C99 and later, an `inline` function — a hint to the compiler that a small function may be expanded at the call site for performance, without the macro's textual hazards) is almost always the better choice. Macros remain useful in C primarily for the things only the preprocessor can do: conditional compilation and stringizing/token-pasting tricks beyond this chapter's scope.

## Conditional Compilation

The preprocessor can include or exclude blocks of code based on conditions evaluated before compilation, using `#ifdef`, `#ifndef`, `#if`, `#elif`, `#else`, and `#endif`.

- `#ifdef NAME` — true if `NAME` has been `#define`d (with any value, or no value at all).
- `#ifndef NAME` — true if `NAME` has *not* been `#define`d; the opposite of `#ifdef`.
- `#if expression` — true if the constant expression evaluates to nonzero, allowing comparisons (e.g. `#if VERSION >= 2`).
- `#elif`, `#else` — work like `else if`/`else`, but for preprocessor conditions.
- `#endif` — closes the conditional block.

A common practical use is compiling extra diagnostic code only in debug builds:

```c
#include <stdio.h>

#define DEBUG

int main(void) {
    int total = 0;

    for (int i = 1; i <= 5; i++) {
        total += i;

#ifdef DEBUG
        printf("After adding %d, total = %d\n", i, total);
#endif
    }

    printf("Final total: %d\n", total);
    return 0;
}
```

### Compile and run

```bash
gcc -std=c11 -Wall -Wextra debugdemo.c -o debugdemo
./debugdemo
```

### Expected output

```text
After adding 1, total = 1
After adding 2, total = 3
After adding 3, total = 6
After adding 4, total = 10
After adding 5, total = 15
Final total: 15
```

If the `#define DEBUG` line is removed (or commented out), the `printf` calls inside the `#ifdef DEBUG` block are excluded entirely *before compilation* — not skipped at runtime, but never compiled into the program at all. This is a common way to ship a "release" build without diagnostic output, and a "debug" build with it, from the very same source file. The same effect can also be achieved without editing the source, by defining the macro on the compiler command line:

```bash
gcc -std=c11 -Wall -Wextra -DDEBUG debugdemo.c -o debugdemo
```

The `-D` flag defines a macro from the command line, exactly as if a corresponding `#define` appeared at the top of the file.

## Include Guards

If a header file is `#include`d more than once while building a single compiled file — which can easily happen indirectly, when two different headers you include both happen to include a third common header — its contents would be duplicated, typically causing "redefinition" errors at compile time. The standard fix is an **include guard**: wrapping the entire contents of a header file in a conditional that ensures it is only processed once.

```c
#ifndef MYHEADER_H
#define MYHEADER_H

/* declarations go here */

#endif
```

The first time this header is included, `MYHEADER_H` is not yet defined, so `#ifndef MYHEADER_H` is true: the body runs, immediately defining `MYHEADER_H` and then providing the header's actual content. If the same header is included again later in the same compilation, `MYHEADER_H` is now already defined, so `#ifndef MYHEADER_H` is false, and the preprocessor skips straight to `#endif` — the contents are not processed a second time. Writing and using your own multi-file headers, where this pattern becomes essential, is covered fully in Chapter 19.

A widely supported, shorter alternative is `#pragma once`, placed once at the top of a header file:

```c
#pragma once

/* declarations go here */
```

`#pragma once` is not part of the official C standard, but essentially every modern compiler (including gcc) supports it, and it is common in practice because it is shorter and cannot be accidentally broken by a typo in a guard macro's name. The classic `#ifndef`/`#define`/`#endif` pattern remains the portable, standard-guaranteed choice.

## Predefined Macros

The preprocessor automatically defines several macros that report information about the current source location and build, useful for diagnostics and logging.

| Macro | Meaning |
|---|---|
| `__FILE__` | The current source file's name, as a string. |
| `__LINE__` | The current line number, as an integer. |
| `__DATE__` | The compilation date, as a string (e.g. `"Jun 22 2026"`). |
| `__TIME__` | The compilation time, as a string. |
| `__func__` | The name of the current function, as a string (this one is a C language feature available inside any function, technically not a preprocessor macro, but used the same way). |

```c
#include <stdio.h>

void report(void) {
    printf("Inside function: %s\n", __func__);
    printf("File: %s, Line: %d\n", __FILE__, __LINE__);
}

int main(void) {
    printf("Compiled on %s at %s\n", __DATE__, __TIME__);
    report();
    return 0;
}
```

### Compile and run

```bash
gcc -std=c11 -Wall -Wextra predefined.c -o predefined
./predefined
```

### Expected output

```text
Compiled on Jun 22 2026 at 09:45:12
Inside function: report
File: predefined.c, Line: 5
```

!!! note "Exact output varies"
    `__DATE__` and `__TIME__` reflect the actual moment you compile, and `__LINE__` reflects the literal line number in your own copy of the file, so your exact output will differ from the listing above — only the pattern is fixed.

### A Poor-Man's Assert/Logging Macro

Combining a function-like macro with `__FILE__`, `__LINE__`, and `__func__` gives a compact diagnostic tool that automatically reports exactly where a message came from:

```c
#include <stdio.h>

#define LOG(message) \
    printf("[%s:%d in %s()] %s\n", __FILE__, __LINE__, __func__, message)

void process(int value) {
    if (value < 0) {
        LOG("negative value encountered");
        return;
    }
    printf("Processing %d\n", value);
}

int main(void) {
    process(5);
    process(-3);
    return 0;
}
```

### Compile and run

```bash
gcc -std=c11 -Wall -Wextra logmacro.c -o logmacro
./logmacro
```

### Expected output

```text
Processing 5
[logmacro.c:8 in process()] negative value encountered
```

The backslash `\` at the end of the `#define` line is the standard way to continue a macro definition onto a second line; the preprocessor joins the two lines into one before substitution. Each call to `LOG(...)` automatically expands with the *exact* file, line, and function of that call site — information a regular function could not capture on its own without being passed it explicitly, which is one of the few genuine advantages function-like macros retain over plain functions.

## `#pragma`: A Brief Mention

`#pragma` is a directive that gives compiler-specific instructions not covered by the standard preprocessor directives — for example, suppressing a particular warning, controlling struct packing, or (as already seen) `#pragma once`. Exactly which pragmas exist, and what they do, depends entirely on the compiler; a pragma gcc understands may be silently ignored, or behave differently, in another compiler. This tutorial will not go deeper into specific pragmas beyond noting that they exist as an escape hatch for compiler-specific behavior.

## Summary

- The preprocessor runs before compilation and operates purely on text, with no knowledge of C types or semantics; `gcc -E` shows the result of preprocessing alone.
- `#include <...>` is for system headers; `#include "..."` is for local/project headers, searched starting in the current directory; writing your own headers is covered fully in Chapter 19.
- `#define NAME value` performs textual substitution with no type checking; prefer `const` (single typed constants) or `enum` (related integer constants) over `#define` for plain constants in modern C.
- Function-like macros (`#define SQUARE(x) ((x) * (x))`) require careful parenthesization of both the parameter and the whole expression to avoid precedence bugs, and can silently evaluate side-effecting arguments multiple times — both reasons real (or `inline`) functions are usually preferable.
- `#ifdef`/`#ifndef`/`#if`/`#elif`/`#else`/`#endif` conditionally include or exclude code at compile time, commonly used for debug-only diagnostics, controllable from the command line with `-D`.
- Include guards (`#ifndef HEADER_H` / `#define HEADER_H` / `#endif`) prevent a header's contents from being processed more than once; `#pragma once` is a widely supported, shorter, non-standard alternative.
- Predefined macros `__FILE__`, `__LINE__`, `__DATE__`, `__TIME__`, and `__func__` report source location and build information, useful for logging and diagnostics.
- `#pragma` issues compiler-specific instructions whose exact meaning and support vary by compiler.

## Exercises

1. Write a program that defines `#define CM_PER_INCH 2.54` and uses it to convert a few inch values (your choice) to centimeters, printing the results.
2. Take the broken `SQUARE(x) x * x` macro from this chapter and write three more test expressions besides `SQUARE(2 + 3)` that also produce wrong results because of missing parentheses; predict each result by hand first, then verify by compiling and running.
3. Write a function-like macro `MAX(a, b)` that expands to the larger of two values using the ternary operator (recall Chapter 4), fully parenthesized to avoid precedence bugs. Test it with plain values, then test it with `MAX(i++, j++)` for two int variables and explain, referencing this chapter's side-effect pitfall, why the result is unreliable.
4. Write a header file `geometry.h` with an include guard containing a `#define PI 3.14159` and a function-like macro for computing a circle's area, then write a `.c` file that includes it twice (directly, with two `#include "geometry.h"` lines) and confirm it still compiles cleanly because of the guard.
5. *(Harder)* Write a program with two build configurations controlled by a `VERBOSE` macro defined (or not) via `-DVERBOSE` on the gcc command line: when defined, the program prints detailed step-by-step progress through a small calculation; when not defined, it prints only the final result. Compile and run it both ways and show both outputs.
6. *(Harder)* Write your own `LOG_ERROR(message)` macro similar to this chapter's `LOG` macro, but using `__LINE__` and `__FILE__` only (not `__func__`), and use it inside at least three different functions in one program to demonstrate that each call site reports its own correct file and line number.

---
[← Chapter 14 — File Handling](chapter-14.md) | [Chapter 16 — Storage Classes, Scope and Recursion →](chapter-16.md)
