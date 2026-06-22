# Chapter 3 — Variables, Data Types and Constants

## Introduction

Now that you can compile and run a basic program, it's time to make your programs actually *do* something with data. This chapter introduces variables (named storage for values), the different kinds of values C can store, and how to create values that are never allowed to change.

## What Is a Variable?

Recall the "labeled boxes" mental model from Chapter 0: a computer's memory is like a long row of storage boxes, each capable of holding a piece of data. A **variable** is a named box: a location in memory that you refer to by a name you choose, which holds a value that your program can read or change while it runs.

When you write:

```c
int age = 30;
```

you are telling the compiler: "Reserve a box big enough to hold an integer. Label that box `age`. Put the value `30` into it right now." From that point on, anywhere in scope (we'll define "scope" properly in Chapter 8 when we cover functions), writing `age` in your code means "go look in that box and use whatever value is currently there."

Crucially, every variable in C has a **type**, fixed at the moment you create it, which determines what *kind* of value it can hold (a whole number, a fraction, a single character, and so on) and how much memory it occupies. C will not let you put a different kind of value into a box than the type it was declared with, without an explicit conversion (we'll cover conversions later in this chapter and again in Chapter 4).

## Declaration vs. Definition vs. Initialization

These three related terms describe slightly different things, and beginners often (understandably) blur them together.

**Declaration** announces a variable's name and type to the compiler, without necessarily setting a value:

```c
int x;
```

This reserves a box labeled `x`, sized to hold an `int`, but the value currently inside it is **indeterminate** — essentially whatever leftover bits happened to already be in that memory location. Reading `x` before giving it a real value produces unpredictable results and is a bug you should always avoid.

**Initialization** means giving a variable its first value at the moment it is declared:

```c
int x = 5;
```

Here, the box is created and immediately filled with `5` in one step. This is, in almost every case, what you should do — declaring a variable without initializing it is a common source of bugs.

**Definition** is a term you'll see used (especially once we reach functions in Chapter 8) to mean "the declaration that actually causes memory to be set aside" — for ordinary variables like the ones in this chapter, declaration and definition happen to be the same act, so the distinction matters more once we discuss functions and multi-file programs later in this tutorial. For now, it's enough to recognize: `int x;` declares (and defines) `x` without a value; `int x = 5;` declares, defines, *and* initializes it in one line.

You can also assign a value to an already-declared variable afterward, separately from initialization:

```c
#include <stdio.h>

int main(void) {
    int x;       // declaration, no value yet
    x = 5;       // assignment, after the fact
    printf("%d\n", x);
    return 0;
}
```

### Compile and run

```bash
gcc -std=c11 -Wall -Wextra demo.c -o demo
./demo
```

### Expected output

```text
5
```

## Primitive Types

C provides a small set of built-in, **primitive types** — the basic kinds of values the language understands natively, without you having to define anything yourself.

**`int`** — a whole number (no fractional part), such as `42` or `-7`.

**`float`** — a "single-precision" floating-point number: a value that can have a fractional part, such as `3.14`, stored using a format that trades off some precision for using less memory.

**`double`** — a "double-precision" floating-point number: similar to `float` but stored with roughly twice the precision (hence the name), and the more common default choice for fractional values in C unless you have a specific reason to prefer `float` (such as conserving memory in large collections of numbers).

**`char`** — a single character, such as `'A'` or `'7'` (note the single quotes — we'll explain this distinction from double-quoted text shortly). Internally, a `char` is actually stored as a small integer, using a standardized mapping called **ASCII** (or an extension of it) where, for example, the letter `'A'` is stored as the number 65. We will rely on this character-to-number relationship more when we cover strings in a later chapter.

**`_Bool`** — a type that holds only a true/false value (technically stored as `0` for false or `1` for true). Writing `_Bool` directly is unusual in practice; instead, C provides a header, `<stdbool.h>`, which lets you write the more familiar `bool`, `true`, and `false`:

```c
#include <stdio.h>
#include <stdbool.h>

int main(void) {
    bool is_ready = true;
    printf("%d\n", is_ready);
    return 0;
}
```

### Compile and run

```bash
gcc -std=c11 -Wall -Wextra bool_demo.c -o bool_demo
./bool_demo
```

### Expected output

```text
1
```

Note that `printf("%d\n", is_ready)` prints `1`, not the word `true` — C has no built-in way to print `true`/`false` as text; it just shows the underlying number. This is a small but important reminder that `bool` is, under the hood, still just an integer with the value restricted to 0 or 1.

**`void`** — this one is different from the others: `void` means "no type" or "no value." You can't create a variable of type `void` (there's no such thing as a "no-value box"). Its main use, which we'll cover properly in Chapter 8, is to describe a function that doesn't return any value, or — as you already saw in `int main(void)` — to describe a function that takes no input values.

## Type Modifiers

Beyond the basic types, C provides **modifiers** that adjust how much memory an integer type uses, and whether it can represent negative numbers.

- **`short`** — typically a smaller integer than plain `int`.
- **`long`** — typically a larger integer than plain `int`.
- **`long long`** — typically larger still.
- **`signed`** — can represent both negative and positive numbers (this is the default for `int` if you don't specify otherwise).
- **`unsigned`** — can represent only zero and positive numbers, but in exchange, can represent a larger maximum positive value using the same amount of memory.

These can combine, for example: `unsigned long long`, `short int`, `unsigned int`.

!!! warning "Sizes are platform- and compiler-dependent"
    The C standard guarantees only *minimum* sizes and *relative orderings* (e.g., `long` is at least as big as `int`) — it does not guarantee exact sizes in bytes. The table below shows typical values on a common modern 64-bit Linux system using gcc, but you should always verify actual sizes on your own system using `sizeof` (introduced next) rather than assuming.

| Type | Typical size (bytes) | Typical range |
|---|---|---|
| `char` | 1 | -128 to 127 (signed) or 0 to 255 (unsigned) |
| `short` | 2 | -32,768 to 32,767 |
| `int` | 4 | -2,147,483,648 to 2,147,483,647 |
| `unsigned int` | 4 | 0 to 4,294,967,295 |
| `long` | 8 | approx. ±9.2 × 10¹⁸ |
| `long long` | 8 | approx. ±9.2 × 10¹⁸ |
| `float` | 4 | approx. ±3.4 × 10³⁸ (about 6-7 significant decimal digits) |
| `double` | 8 | approx. ±1.8 × 10³⁰⁸ (about 15-16 significant decimal digits) |

## The `sizeof` Operator

`sizeof` is an **operator** (a built-in capability of the language, not a regular function) that tells you how many bytes a type — or an existing variable — occupies in memory. (A byte is a unit of memory, conventionally 8 bits, where a bit is a single binary digit, 0 or 1.)

```c
#include <stdio.h>

int main(void) {
    printf("Size of char: %zu byte(s)\n", sizeof(char));
    printf("Size of int: %zu byte(s)\n", sizeof(int));
    printf("Size of long: %zu byte(s)\n", sizeof(long));
    printf("Size of float: %zu byte(s)\n", sizeof(float));
    printf("Size of double: %zu byte(s)\n", sizeof(double));
    return 0;
}
```

### Compile and run

```bash
gcc -std=c11 -Wall -Wextra sizes.c -o sizes
./sizes
```

### Expected output (typical on a 64-bit Linux system; your numbers may differ)

```text
Size of char: 1 byte(s)
Size of int: 4 byte(s)
Size of long: 8 byte(s)
Size of float: 4 byte(s)
Size of double: 8 byte(s)
```

!!! note "Why `%zu`?"
    `sizeof` produces a value of a special unsigned integer type called `size_t`, specifically designed to represent sizes/counts. The matching `printf` format specifier for `size_t` is `%zu`. We'll cover the full table of format specifiers and `printf`/`scanf` mechanics in depth in Chapter 5; for now, just remember `%zu` pairs with `sizeof`.

## Integer Overflow

Every integer type has a fixed amount of memory, and therefore a fixed maximum (and, for signed types, minimum) value it can represent. **Integer overflow** happens when an arithmetic result goes beyond what the type can hold. C does not raise an error when this happens — the value simply "wraps around," because the underlying memory only has so many bits to represent the result in, and the extra information is silently discarded.

```c
#include <stdio.h>

int main(void) {
    unsigned char small_number = 255;  // the maximum an unsigned char can hold
    printf("Before: %u\n", small_number);

    small_number = small_number + 1;   // overflow happens here
    printf("After:  %u\n", small_number);

    return 0;
}
```

### Compile and run

```bash
gcc -std=c11 -Wall -Wextra overflow.c -o overflow
./overflow
```

### Expected output

```text
Before: 255
After:  0
```

An `unsigned char` is typically 1 byte (8 bits), which can represent 256 distinct values: 0 through 255. The number 255, in binary, is `11111111` — all eight bits set. Adding 1 would naturally produce `100000000` (9 bits), but there is no ninth bit available to store in an 8-bit box, so that extra bit is simply lost, and what remains is `00000000`, which is 0. This is why the result "wraps around" back to 0 instead of becoming 256. The same wrapping behavior happens with any unsigned type at its maximum value, and signed integer overflow behaves similarly in practice on most systems, though the C standard actually classifies signed overflow as **undefined behavior** — meaning the language specification places no guarantees on what happens at all, not even predictable wraparound. We will return to the concept of undefined behavior repeatedly throughout this tutorial, since avoiding it is one of the central skills of writing correct C.

!!! danger "No error is raised"
    This is one of the most important things to internalize about C: arithmetic that overflows does not crash your program, print a warning, or stop execution. It silently produces a wrong-looking value, and it is entirely your responsibility, as the programmer, to choose types large enough for the values you expect and to guard against overflow where it matters (for example, in financial or safety-critical calculations).

## Constants

A **constant** is a value that either cannot, or should not, change.

**Literal constants** are values written directly in your code: `42` (an integer literal), `3.14f` (a `float` literal — the trailing `f` tells the compiler to treat this as `float` rather than the default `double`), `'A'` (a `char` literal, in single quotes), and `"hello"` (a string literal, in double quotes — note that in C, a string is really an array of characters, which we will cover properly in a later chapter; for now, just know that double quotes mean "this is text," while single quotes mean "this is exactly one character").

**The `const` keyword** can be applied to a variable declaration to tell the compiler "this variable's value must never change after initialization." The compiler will then produce an error if your code ever tries to assign a new value to it.

```c
#include <stdio.h>

int main(void) {
    const double pi = 3.14159;
    printf("Pi is approximately %f\n", pi);

    // pi = 3.0; // Uncommenting this line would cause a compiler error,
                 // because pi is declared const.

    return 0;
}
```

### Compile and run

```bash
gcc -std=c11 -Wall -Wextra const_demo.c -o const_demo
./const_demo
```

### Expected output

```text
Pi is approximately 3.141590
```

C also offers another, older way to create constants using the preprocessor: `#define PI 3.14159`. This works very differently under the hood from `const` — it is a pure text-substitution instruction handled before compilation even begins, rather than a typed variable. We will cover `#define` and the rest of the preprocessor in full detail in Chapter 15. For now, prefer `const` for typed constant values, since it gives the compiler more information and produces clearer error messages.

## Identifier Naming Rules and Conventions

An **identifier** is any name you choose for a variable, function, or other named entity in your program. C imposes a few hard rules, enforced by the compiler:

- Identifiers may contain letters (`a`-`z`, `A`-`Z`), digits (`0`-`9`), and underscores (`_`).
- They cannot start with a digit (`2cool` is invalid; `cool2` is fine).
- They are case-sensitive (`total` and `Total` are different identifiers).
- They cannot be a **reserved keyword** — a word C itself uses for its own grammar, such as `int`, `return`, `if`, `while`, `const`, and similar words you'll learn throughout this tutorial. You simply cannot name a variable `int` or `return`.

Beyond these hard rules, C programmers follow informal **naming conventions** — not enforced by the compiler, but important for writing code other humans (including future you) can read easily. The most common convention in C is **snake_case**: lowercase words separated by underscores, such as `total_score`, `is_ready`, `max_retry_count`. This tutorial will use snake_case consistently. Choose descriptive names — `total_score` communicates intent far better than a vague name like `x` or `t`, except for very short-lived, conventionally-understood variables like loop counters (which we'll meet in Chapter 6).

## Format Specifiers: A Quick Reference Table

You've already seen `%d` and `%f` used with `printf`. A **format specifier** is a placeholder inside a format string that tells `printf` (when printing) or `scanf` (when reading input) what type of value to expect at that position. Chapter 5 covers `printf` and `scanf` mechanics in full detail; for now, here is a reference table matching the types from this chapter to their most common specifiers, so you can use them confidently in upcoming exercises.

| Type | `printf`/`scanf` specifier | Notes |
|---|---|---|
| `int` | `%d` | "d" stands for "decimal." |
| `unsigned int` | `%u` | |
| `long` | `%ld` | |
| `long long` | `%lld` | |
| `float` (with `printf`) | `%f` | `printf` automatically promotes `float` arguments to `double`. |
| `double` (with `printf`) | `%f` | |
| `double` (with `scanf`) | `%lf` | `scanf` requires the explicit `l` for `double`; this is a common pitfall. |
| `char` | `%c` | |
| string (`char` array) | `%s` | Covered fully once we reach arrays and strings. |
| `size_t` (e.g., from `sizeof`) | `%zu` | |

## Summary

- A variable is a named, typed location in memory; think of it as a labeled storage box holding one value of a fixed type.
- Declaration introduces a name and type (`int x;`); initialization gives it a starting value at creation (`int x = 5;`); assignment (`x = 5;`) can change a variable's value afterward.
- Primitive types: `int` (whole numbers), `float`/`double` (fractional numbers, with `double` offering more precision), `char` (a single character, stored as a small integer), `_Bool`/`bool` (true/false), and `void` (no type, used for functions — Chapter 8).
- Modifiers (`short`, `long`, `long long`, `signed`, `unsigned`) adjust an integer type's size and range; exact sizes are platform/compiler dependent and should be checked with `sizeof`, not assumed.
- Integer overflow wraps values around silently, with no error raised, because of how binary representation works; signed overflow is technically undefined behavior in C.
- `const` creates a variable whose value cannot be changed after initialization; `#define` can also create constants, with full detail deferred to Chapter 15.
- Identifiers must start with a letter or underscore, may contain letters/digits/underscores, are case-sensitive, and cannot be reserved keywords; snake_case is the conventional naming style used throughout this tutorial.
- Common format specifiers: `%d` for `int`, `%f` for `float`/`double` in `printf`, `%lf` for `double` in `scanf`, `%c` for `char`, `%s` for strings, `%zu` for `sizeof` results.

## Exercises

1. Declare three variables — one `int`, one `double`, and one `char` — initialize each with a value of your choosing, and print all three using `printf` with the correct format specifiers.
2. Write a program that declares an `int` without initializing it, then prints its value. Run it two or three times. Does the printed value stay the same or change between runs? What does this tell you about uninitialized variables?
3. Use `sizeof` to print the size, in bytes, of `short`, `int`, `long`, and `long long` on your system. Compare your results to the typical sizes table in this chapter — do they match?
4. Write a program with a `const int max_score = 100;`, then attempt to assign `max_score = 50;` later in the program. Compile it and read the resulting error message.
5. *(Harder)* Write a program that starts an `int` variable at a value close to its maximum (you can find the maximum for `int` by checking `<limits.h>`'s `INT_MAX`, or simply use a large literal like `2147483647`), adds 1 to it, and prints the result before and after. Explain the output in terms of binary representation, the same way this chapter explained the `unsigned char` overflow example.
6. *(Harder)* Write a program that declares a `float` and a `double`, both initialized to the same fractional value with many decimal digits (e.g., `1.0 / 3.0`), and prints both with `printf` using `%f`. Research (or guess, then verify) why their printed precision differs, tying your explanation back to the relative sizes of `float` and `double` shown in this chapter.

---
[← Chapter 2 — Your First C Program and the Compilation Process](chapter-02.md) | [Chapter 4 — Operators and Expressions →](chapter-04.md)
