# Chapter 16 — Storage Classes, Scope, and Recursion

## Introduction

This chapter completes the picture of how variables and functions relate to the source code that declares them and the time during which they actually exist in memory. The first half formalizes **scope** (where a name is visible) and introduces **storage classes** (`static`, `extern`, and briefly `auto`/`register`), which control **linkage** and **lifetime** — concepts distinct from scope that are often confused with it. The second half is devoted to **recursion**, a function calling itself, explored in enough depth to understand exactly how it works, when to use it, and its risks.

## Scope, Revisited and Completed

**Scope** is about the source code: it answers the question "in which parts of the program text is this name visible and usable?" You have already used several kinds of scope informally; here they are named precisely.

- **Block scope**: a variable declared inside `{ ... }` — including a function body, or any nested block such as the body of an `if`, `for`, or `while` — is visible from its declaration to the end of that enclosing block, and nowhere outside it.
- **Function scope**: technically distinct from block scope, this applies only to labels used with `goto` (not covered in this tutorial); for ordinary variables, block scope is what matters.
- **File scope**: a variable or function declared outside of any function, at the top level of a source file, is visible from its declaration to the end of that file. These are commonly called **global** variables.
- **Global scope**: in casual usage, "global" is used interchangeably with file-scope variables and functions defined outside any function — visible throughout the file from their point of declaration, and (depending on linkage, discussed below) potentially in other files too.

### Nested Blocks and Shadowing

A variable declared in an inner block can have the same name as a variable in an enclosing block. Inside the inner block, the inner variable **shadows** (temporarily hides) the outer one; the outer variable is unaffected and reappears once the inner block ends.

```c
#include <stdio.h>

int main(void) {
    int x = 10;
    printf("Outer x = %d\n", x);

    {
        int x = 99;
        printf("Inner x = %d\n", x);
    }

    printf("Outer x again = %d\n", x);

    return 0;
}
```

### Compile and run

```bash
gcc -std=c11 -Wall -Wextra shadow.c -o shadow
./shadow
```

### Expected output

```text
Outer x = 10
Inner x = 99
Outer x again = 10
```

Inside the inner `{ }` block, `x` refers to the freshly declared inner variable; the outer `x` still exists in memory the whole time, but its name is temporarily inaccessible within the inner block. Once the inner block ends, the inner `x` ceases to exist, and the name `x` again refers to the outer variable, unchanged.

!!! warning "Shadowing is legal but often a mistake"
    Most compilers, including gcc with `-Wall -Wextra` or the stronger `-Wshadow` flag, can warn about shadowed variables, because accidental shadowing — intending to modify the outer variable but actually creating and modifying an unrelated inner one — is a common source of confusing bugs. Use distinct names for inner and outer variables unless you have a deliberate, clear reason to shadow.

## Storage Classes

A **storage class** is a keyword that controls two things at once: how long a variable's storage lasts (its lifetime), and, for names visible outside their own file, what other files are allowed to refer to them (linkage). C provides four storage class keywords.

### `auto` — the Default, Rarely Written

Every local variable you have declared since Chapter 3 — `int x;` inside a function — is implicitly `auto`: automatically allocated when its block begins and automatically deallocated when its block ends. You could write `auto int x;`, but in practice nobody does, since it is the default and the keyword adds nothing. It is mentioned here mainly so the full set of storage classes is complete, and so that the contrast with `static` below is clear.

### `register` — A Historical Hint, Rarely Used Today

`register` suggests to the compiler that a variable will be accessed very frequently and might benefit from being kept in a CPU register rather than ordinary memory, for speed.

```c
register int counter;
```

Modern optimizing compilers are almost always better at deciding which variables belong in registers than a programmer guessing by hand, so `register` is rarely used in modern code, and compilers are free to ignore the hint entirely. It is included here for completeness and historical context, not as a technique to reach for.

### `static` on a Local Variable: Persisting Between Calls

Normally, a local variable's lifetime is tied strictly to one call of its function: it is created fresh (with garbage or initializer value) every time the function is called, and destroyed when the function returns — the next call starts over with no memory of the previous one. Declaring a local variable `static` changes this: it is allocated exactly once, before the program starts running, retains its value between calls, and is only initialized once (the first time control reaches its declaration), not on every call.

```c
#include <stdio.h>

void track_calls(void) {
    static int call_count = 0;
    call_count++;
    printf("This function has been called %d time(s).\n", call_count);
}

int main(void) {
    track_calls();
    track_calls();
    track_calls();
    return 0;
}
```

### Compile and run

```bash
gcc -std=c11 -Wall -Wextra staticlocal.c -o staticlocal
./staticlocal
```

### Expected output

```text
This function has been called 1 time(s).
This function has been called 2 time(s).
This function has been called 3 time(s).
```

If `call_count` were an ordinary (`auto`) local variable, it would reset to `0` at the start of every call, and the output would print `1` three times. Because it is `static`, the variable's storage is allocated once, for the entire run of the program, in the same general region as global variables — not on the stack, and not re-created on each call — so its value from the previous call is still there the next time the function runs.

### `static` on a Global Variable or Function: Internal Linkage

`static` means something related but distinct when applied at file scope, to a global variable or a function (rather than a local variable inside a function body): it restricts that name's **linkage** to the current file only — meaning no other source file in a multi-file program can refer to it, even if it tries.

```c
#include <stdio.h>

static int internal_counter = 0;

static void increment_internal(void) {
    internal_counter++;
}

int main(void) {
    increment_internal();
    increment_internal();
    printf("internal_counter = %d\n", internal_counter);
    return 0;
}
```

### Compile and run

```bash
gcc -std=c11 -Wall -Wextra staticglobal.c -o staticglobal
./staticglobal
```

### Expected output

```text
internal_counter = 2
```

In a single-file program like this, the practical behavior looks the same with or without `static` — the difference only becomes visible (and important) once a program is split across multiple `.c` files, where `static` at file scope prevents another file from accidentally referring to, or colliding in name with, something meant to be private to this file. That full multi-file picture — how separate `.c` files are compiled and linked together — is covered in Chapter 19.

### `extern`: Referring to a Definition in Another File

`extern` does roughly the opposite job: rather than restricting a name's visibility, it *declares* that a global variable or function exists and is defined somewhere — typically in another file — without itself creating new storage. This single-file example illustrates the keyword's meaning, even though its real purpose appears only across multiple files (Chapter 19).

```c
#include <stdio.h>

int shared_value = 42;

void show_value(void) {
    extern int shared_value;
    printf("shared_value = %d\n", shared_value);
}

int main(void) {
    show_value();
    return 0;
}
```

### Compile and run

```bash
gcc -std=c11 -Wall -Wextra externdemo.c -o externdemo
./externdemo
```

### Expected output

```text
shared_value = 42
```

Inside `show_value`, `extern int shared_value;` does not create a new variable — it tells the compiler "trust me, a global `int` named `shared_value` is defined somewhere; let me refer to it here." In this single-file example that definition happens to be a few lines above, in the same file, making the `extern` declaration technically unnecessary (the global would be visible anyway) but legal and illustrative. The keyword's real value emerges when the definition (`int shared_value = 42;`) lives in a *different* `.c` file than the code using it — exactly the situation Chapter 19 covers in full.

## Lifetime vs. Scope: Two Different Concepts

Scope and lifetime are frequently confused because they often (but not always) line up, so it is worth stating the distinction plainly:

| Concept | Question it answers | Example |
|---|---|---|
| **Scope** | Where, in the *source code text*, is this name visible and usable? | A local variable's scope is its enclosing block — its name means nothing before its declaration or after the closing `}`. |
| **Lifetime** | How long, during *program execution*, does this variable's storage actually exist? | An ordinary local variable's lifetime is one call of its function; a `static` local variable's lifetime is the entire program run; heap memory's lifetime (Chapter 13) lasts until explicitly freed. |

The `static` local variable example above makes the distinction concrete: `call_count`'s **scope** is still just the body of `track_calls` — its name is meaningless anywhere else in the program, exactly like an ordinary local variable. But its **lifetime** is the entire program's execution, not just one call — utterly unlike an ordinary local variable. Scope is a *textual* property fixed once and for all at compile time; lifetime is a *runtime* property describing how long storage persists.

## Recursion

A **recursive** function is a function that calls itself, either directly or indirectly (through another function that calls back to it). Recursion is a natural fit for problems that are themselves naturally defined in terms of smaller versions of themselves.

### The Two Essential Parts: Base Case and Recursive Case

Every correct recursive function needs exactly two parts:

- A **base case**: a condition simple enough to answer directly, without any further recursive call — this is what eventually stops the recursion.
- A **recursive case**: the function calls itself with an argument that is somehow *closer* to the base case than the original call, making guaranteed progress toward stopping.

Without a base case (or with a recursive case that never actually gets closer to it), the function calls itself forever, which in practice ends only when it crashes — discussed in detail later in this chapter.

### Factorial: A First Example

The factorial of `n` (written `n!`) is defined mathematically exactly as a recursion: `0! = 1` (the base case), and for `n > 0`, `n! = n * (n - 1)!` (the recursive case, expressed in terms of a smaller factorial).

```c
#include <stdio.h>

int factorial(int n) {
    if (n == 0) {
        return 1;            /* base case */
    }
    return n * factorial(n - 1);  /* recursive case */
}

int main(void) {
    for (int i = 0; i <= 6; i++) {
        printf("%d! = %d\n", i, factorial(i));
    }
    return 0;
}
```

### Compile and run

```bash
gcc -std=c11 -Wall -Wextra factorial.c -o factorial
./factorial
```

### Expected output

```text
0! = 1
1! = 1
2! = 2
3! = 6
4! = 24
5! = 120
6! = 720
```

### Visualizing the Call Stack: `factorial(4)`

Recall from Chapter 8 that every function call gets its own space on the stack for its local variables and parameters — a **stack frame**. A recursive call is not special in this respect: each call to `factorial`, including calls to itself, gets its own separate stack frame, with its own separate copy of `n`. Tracing `factorial(4)` makes this concrete.

First, the calls go *down*, each one waiting on the next before it can compute anything, since each needs the result of `factorial(n - 1)` before it can multiply by `n`:

```text
factorial(4)  n=4   -- needs factorial(3) to continue
  factorial(3)  n=3   -- needs factorial(2) to continue
    factorial(2)  n=2   -- needs factorial(1) to continue
      factorial(1)  n=1   -- needs factorial(0) to continue
        factorial(0)  n=0   -- base case! returns 1 immediately
```

Once the base case `factorial(0)` returns `1` without making any further call, the stack **unwinds** — each waiting call resumes exactly where it left off, now able to finish its own multiplication, and returns control to whichever call is waiting on it:

```text
        factorial(0) returns 1
      factorial(1) computes 1 * 1 = 1, returns 1
    factorial(2) computes 2 * 1 = 2, returns 2
  factorial(3) computes 3 * 2 = 6, returns 6
factorial(4) computes 4 * 6 = 24, returns 24
```

Five separate stack frames existed briefly, all at once, at the deepest point of this recursion (when `factorial(0)` was executing) — one for each of `factorial(4)`, `factorial(3)`, `factorial(2)`, `factorial(1)`, and `factorial(0)`, each holding its own independent value of `n`. This stacking and unwinding is exactly why the mechanism is called "the call stack," and it is the same stack used for ordinary, non-recursive function calls — recursion simply pushes several frames of the *same* function onto it before any of them return.

### Fibonacci: Naive Recursion and Its Inefficiency

The Fibonacci sequence is defined as `fib(0) = 0`, `fib(1) = 1`, and `fib(n) = fib(n - 1) + fib(n - 2)` for `n >= 2` — another definition that is naturally recursive, with *two* recursive calls per step rather than one.

```c
#include <stdio.h>

int fib(int n) {
    if (n == 0) {
        return 0;   /* base case 1 */
    }
    if (n == 1) {
        return 1;   /* base case 2 */
    }
    return fib(n - 1) + fib(n - 2);  /* recursive case */
}

int main(void) {
    for (int i = 0; i <= 10; i++) {
        printf("fib(%d) = %d\n", i, fib(i));
    }
    return 0;
}
```

### Compile and run

```bash
gcc -std=c11 -Wall -Wextra fibonacci.c -o fibonacci
./fibonacci
```

### Expected output

```text
fib(0) = 0
fib(1) = 1
fib(2) = 1
fib(3) = 2
fib(4) = 3
fib(5) = 5
fib(6) = 8
fib(7) = 13
fib(8) = 21
fib(9) = 34
fib(10) = 55
```

This naive version is correct but strikingly inefficient: computing `fib(n)` calls `fib(n - 1)` *and* `fib(n - 2)`, and each of those makes two further calls, and so on — the number of calls roughly doubles with each increase in `n`, growing exponentially. The deep inefficiency is that the same smaller Fibonacci values get recomputed enormous numbers of times along the way — for example, `fib(5)`'s call tree computes `fib(2)` three separate times, fully redoing identical work each time, and the redundancy grows rapidly worse as `n` increases. For small `n` (as in the example above, up to 10) this is unnoticeable, but calling this exact function with, say, `n = 40` would already take a very long time on ordinary hardware. A technique called **memoization** — remembering previously computed results to avoid recomputing them — fixes this dramatically, but is not covered in this introductory treatment of recursion.

### Greatest Common Divisor: Euclid's Algorithm

Euclid's algorithm computes the greatest common divisor (GCD) of two numbers recursively: `gcd(a, 0) = a` (the base case), and `gcd(a, b) = gcd(b, a % b)` for `b != 0` (the recursive case, using the remainder operator `%` from Chapter 4).

```c
#include <stdio.h>

int gcd(int a, int b) {
    if (b == 0) {
        return a;             /* base case */
    }
    return gcd(b, a % b);     /* recursive case */
}

int main(void) {
    printf("gcd(48, 18) = %d\n", gcd(48, 18));
    printf("gcd(100, 75) = %d\n", gcd(100, 75));
    printf("gcd(17, 5) = %d\n", gcd(17, 5));
    return 0;
}
```

### Compile and run

```bash
gcc -std=c11 -Wall -Wextra gcd.c -o gcd
./gcd
```

### Expected output

```text
gcd(48, 18) = 6
gcd(100, 75) = 25
gcd(17, 5) = 1
```

Each recursive call's second argument (`a % b`) is strictly smaller than the previous call's second argument, guaranteeing the sequence of calls reaches the base case (`b == 0`) in a finite number of steps — exactly the "guaranteed progress toward the base case" property every correct recursive function needs.

## Recursion vs. Iteration

Every recursive function can, in principle, be rewritten using a loop instead (recall `for`/`while` from Chapter 7) — recursion is a choice of *style*, not a capability loops lack. The tradeoffs go both ways:

- **Recursion can be more readable** for problems that are naturally defined recursively — the recursive code for factorial, Fibonacci, and GCD above mirrors their mathematical definitions almost exactly, which is often easier to read and verify than an equivalent loop. Recursion becomes especially natural, and is strongly preferred, for problems with a tree-like or branching structure — traversing the nodes of a tree data structure is a leading example, covered fully in Chapter 18.
- **Recursion has function-call overhead.** Each recursive call is a real function call: a new stack frame, parameters copied, a return address saved — all real work and real memory that a simple loop, which reuses the same variables on every iteration, avoids entirely.
- **Recursion risks stack overflow for deep recursion.** Because every call adds a stack frame, and the stack has a limited, finite size, a recursive function that calls itself too many times — without ever returning to free up frames — can exhaust the stack entirely, crashing the program. A loop has no equivalent limit tied to how many times it repeats.

### Stack Overflow Risk

!!! warning "Unbounded or excessively deep recursion can crash the program"
    A recursive function with a missing or incorrect base case — one that never actually stops calling itself — keeps pushing new stack frames with no end, until the stack's limited space runs out, producing a **stack overflow**: an abrupt crash, often without a clean error message, since the crash happens at the level of the operating system running out of room to grow the stack, beneath the reach of C's own error-handling tools. Even a *correct* base case is not automatically safe if the recursion can go extremely deep before reaching it — for example, computing `factorial` or `fib` recursively for a very large `n` builds up a correspondingly large number of stack frames, each consuming real memory, and can overflow the stack well before any individual frame does anything wrong. This differs from the more contained, recoverable failure of `malloc` returning `NULL` (Chapter 13): a stack overflow typically terminates the entire program immediately, with no opportunity for your code to detect or handle it.

    A function like this, with no base case at all, demonstrates the danger (described here rather than provided as a runnable example, since running it will crash your terminal session):

    ```c
    int runaway(int n) {
        return runaway(n + 1);   /* no base case — never stops */
    }
    ```

    Calling `runaway(0)` begins an unbroken chain of calls that never returns, consuming one stack frame after another until the stack overflows and the program crashes.

Because of this risk, iteration is generally preferred over recursion whenever a problem does not have a naturally recursive structure, or when the recursion depth could become very large — a loop computing factorial or Fibonacci iteratively does the same work with a single stack frame reused throughout, immune to this entire class of failure.

## Summary

- Scope (block, file/global) is about where a name is visible in the source code text; nested blocks can shadow an outer variable of the same name, temporarily hiding it within the inner block.
- Storage classes: `auto` is the (almost always implicit) default for local variables; `register` is a now-rarely-used hint to the compiler; `static` and `extern` matter the most going forward.
- `static` on a local variable makes it persist its value between calls, allocated once rather than recreated each call; `static` on a global variable or function restricts its linkage to the current file (full multi-file treatment in Chapter 19).
- `extern` declares that a global variable or function is defined elsewhere (typically another file), without creating new storage itself; its full purpose appears in multi-file programs (Chapter 19).
- Scope (where a name is visible in the text) and lifetime (how long its storage exists at runtime) are distinct concepts that often, but not always, coincide.
- Recursion requires a base case (stops the recursion) and a recursive case that makes guaranteed progress toward it; the call stack grows with each call and unwinds as each call returns, as traced for `factorial(4)`.
- Naive recursive Fibonacci is correct but exponentially inefficient, since it recomputes the same smaller values repeatedly; memoization (not covered here) fixes this.
- Recursion can be more readable for naturally recursive problems (especially tree-like structures, Chapter 18) but carries function-call overhead and risks stack overflow for deep or unbounded recursion; iteration avoids both costs.

## Exercises

1. Write a program with a nested block (like this chapter's shadowing example) but using three levels of nesting, each declaring a variable named `n` with a different value, and print `n` at each level to confirm each block sees its own.
2. Write a function with a `static` local variable that returns a unique increasing ID number each time it is called (starting from 1), and call it five times from `main`, printing each returned ID.
3. Write a recursive function `int sum_to(int n)` that computes `1 + 2 + ... + n` (base case `n == 0` returns `0`), and trace its call stack by hand for `sum_to(4)` in a comment, in the same style as this chapter's `factorial(4)` diagram, before running it to confirm your trace was correct.
4. Write both a recursive and an iterative version of a function that reverses the digits of a positive integer printed as a number (e.g., 1234 becomes 4321), and confirm both give the same result for several test inputs.
5. *(Harder)* Modify the naive recursive `fib` function to also take and increment a `static` or global call counter, then print how many total calls were needed to compute `fib(20)`. Compare this count to a simple iterative Fibonacci function's "call count" (which is effectively 1, itself, regardless of `n`), and explain in a comment why the recursive version's count grows the way it does.
6. *(Harder)* Write a recursive function `int power(int base, int exponent)` computing `base` raised to `exponent` (assume `exponent >= 0`), with the base case `exponent == 0` returning `1`. Then deliberately call it with a large negative `exponent` (which never reaches the base case, since the recursive case presumably decreases `exponent` toward 0) to observe what kind of failure occurs on your system — describe what you observe in a comment, run it cautiously, and then fix the function to guard against negative exponents instead.

---
[← Chapter 15 — Preprocessor Directives and Macros](chapter-15.md) | [Chapter 17 — Advanced Pointers →](chapter-17.md)
