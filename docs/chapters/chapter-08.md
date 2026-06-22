# Chapter 8 — Functions and Modular Programming

## Introduction

Every program in this tutorial so far has lived entirely inside `main`. That works for small examples, but it does not scale: real programs need to reuse the same logic in multiple places, and they become unreadable if every single operation is crammed into one giant `main`. A **function** is a named, self-contained block of code that performs a specific task, which you can call (invoke) by name as many times as you need. This chapter covers how to declare, define, call, and reason about functions in C — the single most important organizing tool in the language.

## Why Functions Exist

Three closely related motivations drive the use of functions:

- **Avoiding repetition.** If the same calculation or sequence of steps is needed in several places, writing it once as a function and calling it repeatedly avoids duplicating the code (and duplicating any future bug fixes).
- **Abstraction.** A well-named function lets the *caller* think in terms of "compute the greatest common divisor" rather than the specific steps involved — the details are hidden behind the name, and can be understood, tested, and changed independently of the code that uses them.
- **Organization.** Splitting a large program into a set of small, focused functions, each responsible for one clear task, makes the overall structure of the program easier to read, debug, and extend than one undifferentiated block of code.

## Declaration, Definition, and Call

These are three distinct, easily confused concepts:

- A **declaration** (also called a **prototype**) tells the compiler that a function with a given name, return type, and parameter types exists, without providing its body. It ends in a semicolon: `int square(int n);`
- A **definition** provides the actual body — the code that runs when the function is called: `int square(int n) { return n * n; }`
- A **call** is where you actually invoke the function to run it and (optionally) use its result: `int result = square(5);`

```c
#include <stdio.h>

int square(int n); // declaration (prototype)

int main(void) {
    int result = square(5); // call
    printf("5 squared is %d\n", result);
    return 0;
}

int square(int n) { // definition
    return n * n;
}
```

### Compile and run

```bash
gcc -std=c11 -Wall -Wextra square_prototype.c -o square_prototype
./square_prototype
```

### Expected output

```text
5 squared is 25
```

### Why Prototypes Are Needed

The C compiler reads a source file from top to bottom, in a single pass. When it reaches the call `square(5)` inside `main`, it needs to already know `square`'s return type and the number and types of its parameters, in order to generate correct code for the call and check that you're calling it correctly. If `square`'s *definition* appeared only later in the file, the compiler would not yet have seen it at the point of the call — hence the **declaration** earlier in the file, which gives the compiler everything it needs to know without yet needing the full body.

There are two ways to satisfy this requirement:

1. Write a prototype (as above) before `main`, and the full definition afterward — common when you want `main` to appear first, near the top of the file, for readability.
2. Simply write the complete function definition *before* `main`, with no separate prototype at all — the definition itself also tells the compiler everything a prototype would.

```c
#include <stdio.h>

int square(int n) { // full definition, placed before main — no separate prototype needed
    return n * n;
}

int main(void) {
    int result = square(5);
    printf("5 squared is %d\n", result);
    return 0;
}
```

### Compile and run

```bash
gcc -std=c11 -Wall -Wextra square_predefined.c -o square_predefined
./square_predefined
```

### Expected output

```text
5 squared is 25
```

Both programs above produce identical output; they differ only in organization. As programs grow, the common convention is prototypes near the top of the file (or in a header file, in larger multi-file programs) and definitions afterward — this lets `main`, often the most important function to read first, appear early.

!!! note "Connection to the compilation pipeline"
    Recall from Chapter 2 that compilation happens in stages, with a separate **linking** stage that resolves function calls to their actual definitions. A prototype satisfies the *compiler*, which only needs to check types and generate correct call instructions during compilation; the *linker* is what later actually connects each call to where the function's machine code lives, even across separate compiled files. This is why, in larger multi-file programs, a function declared in one file can be defined in another — the compiler is satisfied by a prototype (usually shared via a header file), and the linker does the rest.

## Parameters, Arguments, and Return Values

A **parameter** is a variable listed in a function's definition, describing what input it expects. An **argument** is the actual value supplied at a particular call site. A function's **return value** is the single value it sends back to its caller via the `return` statement; the function's declared return type must match what `return` actually sends back.

```c
#include <stdio.h>

int add(int a, int b) { // a and b are parameters
    return a + b;
}

int main(void) {
    int sum = add(3, 4); // 3 and 4 are arguments
    printf("Sum: %d\n", sum);
    return 0;
}
```

### Compile and run

```bash
gcc -std=c11 -Wall -Wextra add_function.c -o add_function
./add_function
```

### Expected output

```text
Sum: 7
```

### `void` Functions

A function that performs an action but does not need to send any value back to its caller is declared with return type `void`, and its `return;` statement (if used at all) takes no value.

```c
#include <stdio.h>

void print_separator(void) {
    printf("----------------------------\n");
}

int main(void) {
    printf("Section 1\n");
    print_separator();
    printf("Section 2\n");
    print_separator();
    printf("Section 3\n");

    return 0;
}
```

### Compile and run

```bash
gcc -std=c11 -Wall -Wextra void_function.c -o void_function
./void_function
```

### Expected output

```text
Section 1
----------------------------
Section 2
----------------------------
Section 3
```

`print_separator` takes no parameters (hence `void` inside its parentheses, same meaning as in `main(void)` from Chapter 2) and returns nothing (hence `void` as its return type) — it exists purely for the side effect of printing.

## Pass-by-Value, Explained Carefully

This is one of the most important — and most frequently misunderstood — rules in C: **when you pass a variable to a function, the function receives a copy of that variable's value, not the variable itself.** Changes made to a parameter inside the function have no effect whatsoever on the caller's original variable.

```c
#include <stdio.h>

void try_to_double(int n) {
    n = n * 2; // modifies only the local copy
    printf("Inside try_to_double, n is now: %d\n", n);
}

int main(void) {
    int value = 5;

    try_to_double(value);

    printf("Back in main, value is still: %d\n", value);

    return 0;
}
```

### Compile and run

```bash
gcc -std=c11 -Wall -Wextra pass_by_value.c -o pass_by_value
./pass_by_value
```

### Expected output

```text
Inside try_to_double, n is now: 10
Back in main, value is still: 5
```

Even though `n` was successfully doubled *inside* the function, `value` in `main` is completely untouched. `n` and `value` are two entirely separate pieces of memory; assigning `value` to `n` as an argument copied the number `5` into `n`, and nothing afterward connects them.

### The Failed Swap

This consequence becomes very concrete with an attempt to write a function that swaps two variables:

```c
#include <stdio.h>

void swap_attempt(int a, int b) {
    int temp = a;
    a = b;
    b = temp;
    printf("Inside swap_attempt: a = %d, b = %d\n", a, b);
}

int main(void) {
    int x = 1;
    int y = 2;

    printf("Before swap_attempt: x = %d, y = %d\n", x, y);
    swap_attempt(x, y);
    printf("After swap_attempt:  x = %d, y = %d\n", x, y);

    return 0;
}
```

### Compile and run

```bash
gcc -std=c11 -Wall -Wextra failed_swap.c -o failed_swap
./failed_swap
```

### Expected output

```text
Before swap_attempt: x = 1, y = 2
Inside swap_attempt: a = 2, b = 1
After swap_attempt:  x = 1, y = 2
```

The swap genuinely happens — but only to `a` and `b`, the function's own local copies. `x` and `y` in `main` are completely unaffected, exactly as pass-by-value predicts: `a` and `b` were initialized from copies of `x` and `y`'s values, and nothing the function does to `a` and `b` can reach back into `main`'s memory.

!!! note "How do you actually swap the caller's variables, then?"
    To let a function modify a variable that belongs to its caller, the function needs more than a copy of the value — it needs the variable's **address**, so it can reach into the caller's own memory and write there directly. An address-holding variable is called a **pointer**, and pointers are the entire subject of Chapter 11, where this exact swap example is revisited and finally fixed, using a function signature like `void swap(int *a, int *b)`.

## Variable Scope: A Brief Teaser

A variable's **scope** is the region of code where it is visible and usable. A variable declared inside a function (including its parameters) is called a **local variable** — it exists only within that function and is completely inaccessible from any other function. A variable declared outside every function is a **global variable** — it is visible to every function in the file from its declaration point onward.

```c
#include <stdio.h>

int global_counter = 0; // global variable: visible to every function below

void increment_counter(void) {
    global_counter = global_counter + 1; // no parameter needed; reads/writes the global directly
}

int main(void) {
    increment_counter();
    increment_counter();
    increment_counter();

    printf("global_counter is now: %d\n", global_counter);

    return 0;
}
```

### Compile and run

```bash
gcc -std=c11 -Wall -Wextra scope_teaser.c -o scope_teaser
./scope_teaser
```

### Expected output

```text
global_counter is now: 3
```

Notice `increment_counter` modifies `global_counter` directly, with no parameter and no return value, and that modification *is* visible afterward in `main` — global variables are the one major exception to the "functions only see copies" rule, precisely because a global variable isn't local to any one function in the first place.

!!! warning "Use global variables sparingly"
    Relying heavily on global variables makes programs harder to reason about, since any function can silently change shared state that every other function also depends on. For now, just recognize the distinction between local and global; full detail on scope rules, along with the `static` and `extern` keywords that give even finer control over visibility and lifetime, is covered in Chapter 16.

## A First Look at Recursion

A function is **recursive** if it calls itself. The classic introductory example is computing a factorial (`n! = n * (n-1) * (n-2) * ... * 1`), which has a natural recursive definition: `n!` equals `n * (n-1)!`, with the special case `0! = 1` providing a place for the calls to stop.

```c
#include <stdio.h>

int factorial(int n) {
    if (n == 0) {
        return 1; // base case: stops the recursion
    }
    return n * factorial(n - 1); // recursive case: calls itself with a smaller input
}

int main(void) {
    int number = 5;
    printf("%d! = %d\n", number, factorial(number));

    return 0;
}
```

### Compile and run

```bash
gcc -std=c11 -Wall -Wextra factorial_recursive.c -o factorial_recursive
./factorial_recursive
```

### Expected output

```text
5! = 120
```

`factorial(5)` calls `factorial(4)`, which calls `factorial(3)`, and so on down to `factorial(0)`, which returns `1` immediately without calling itself again; each call then multiplies its own `n` by the result it received, unwinding back up to the original call. This is "a function calling itself" in its simplest possible form.

!!! note "There's much more to recursion than this"
    This example only scratches the surface. A full treatment of recursion, the **call stack** (how the computer actually keeps track of all these nested, in-progress calls), and a discussion of when recursion is the right tool to reach for versus a simple loop, is the subject of Chapter 16. For now, it's enough to recognize the pattern and understand this one example.

## Worked Example: A Small Math Utility Library

Here is a complete program collecting several small, focused functions — each declared, defined, and called within one file — demonstrating how a program can be organized as a set of cooperating pieces rather than one undifferentiated block of code in `main`.

```c
#include <stdio.h>

// Prototypes
int is_even(int n);
int gcd(int a, int b);
long power(int base, int exponent);

int main(void) {
    int x = 18;
    int y = 24;
    int base = 2;
    int exponent = 10;

    printf("Is %d even? %s\n", x, is_even(x) ? "yes" : "no");
    printf("gcd(%d, %d) = %d\n", x, y, gcd(x, y));
    printf("%d raised to the power %d = %ld\n", base, exponent, power(base, exponent));

    return 0;
}

int is_even(int n) {
    return n % 2 == 0;
}

int gcd(int a, int b) {
    while (b != 0) {
        int remainder = a % b;
        a = b;
        b = remainder;
    }
    return a;
}

long power(int base, int exponent) {
    long result = 1;
    for (int i = 0; i < exponent; i++) {
        result = result * base;
    }
    return result;
}
```

### Compile and run

```bash
gcc -std=c11 -Wall -Wextra math_utils.c -o math_utils
./math_utils
```

### Expected output

```text
Is 18 even? yes
gcd(18, 24) = 6
2 raised to the power 10 = 1024
```

`is_even` returns the `int` result of a comparison directly (recall from Chapter 6 that comparisons evaluate to `1` or `0`), used here with the ternary operator from Chapter 4 to select between `"yes"` and `"no"`. `gcd` implements the Euclidean algorithm using a `while` loop (Chapter 7). `power` uses a `for` loop and returns `long` to comfortably hold larger results. Each function is independently understandable and independently testable, and `main` reads almost like a summary of what the program does, with the details delegated to clearly named helpers.

## Summary

- Functions exist to avoid repeating code, to let callers reason at a higher level of abstraction, and to organize a program into clear, focused pieces.
- A declaration (prototype) tells the compiler a function's signature in advance; a definition provides its body; a call invokes it. Prototypes are needed because C compiles top to bottom in one pass, and the linker (Chapter 2) later resolves calls to their actual definitions.
- Parameters are placeholders in a definition; arguments are the actual values supplied at a call. `void` marks a function (or its parameter list) as having no return value (or no parameters).
- C is strictly pass-by-value: a function receives a copy of each argument, and modifying a parameter never affects the caller's original variable — demonstrated concretely by the failed-swap example. Modifying a caller's variable requires passing its address (a pointer), the subject of Chapter 11.
- Local variables exist only within their function; global variables, declared outside every function, are visible everywhere from their declaration onward — full scope rules, plus `static` and `extern`, are covered in Chapter 16.
- A recursive function calls itself; `factorial` is the canonical simple example, with full coverage of the call stack and when to use recursion deferred to Chapter 16.

## Exercises

1. Write a function `int max_of_two(int a, int b)` that returns the larger of its two arguments, and call it from `main` with several pairs of test values.
2. Write a function `void print_stars(int n)` that prints `n` asterisks followed by a newline, and use it in a loop in `main` to print a triangle pattern (recall Chapter 7).
3. Write a function `int is_even(int n)` (if you haven't already from the worked example) and a complementary `int is_odd(int n)` that calls `is_even` internally rather than recomputing the check itself.
4. Modify the failed-swap example: add print statements immediately before and after the call to `swap_attempt` showing the addresses of `x` and `y` using `%p` (recall this format specifier from Chapter 5), and the addresses of `a` and `b` printed from inside the function. Confirm the addresses are different, and explain in a comment why that fact alone proves the swap could never have worked.
5. *(Harder)* Write a recursive function `int sum_to_n(int n)` that returns `1 + 2 + ... + n` by calling itself, with `sum_to_n(0)` returning `0` as the base case. Trace through, on paper, exactly what calls are made for `sum_to_n(4)` before writing the code.
6. *(Harder)* Extend the math utility library worked example with a new function `int is_prime(int n)` (recall the logic from Chapter 7's prime-checking example, but now packaged as a reusable function), and use it in `main` to print all prime numbers between 2 and 50.

---
[← Chapter 7 — Loops and Iteration](chapter-07.md) | [Chapter 9 — Arrays →](chapter-09.md)
