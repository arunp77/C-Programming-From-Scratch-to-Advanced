# Chapter 11 — Pointers: Fundamentals

## Introduction

Pointers have been mentioned and deferred repeatedly throughout this tutorial: the `&` in `scanf("%d", &age)` back in Chapter 5, the explanation of why the failed-swap function in Chapter 8 couldn't modify its caller's variables, and the explanation of array decay in Chapter 9. All of these are finally resolved in this chapter. A **pointer** is simply a variable that holds a memory address — nothing more mysterious than that — but the consequences of being able to hold and use an address turn out to be central to how C programs actually work.

## What a Pointer Is: An Analogy

Think of a variable as a house: it has contents (the value stored inside it) and a location (its address). An ordinary variable like `int age = 30;` is the house itself — when you use `age` in an expression, you're using what's inside the house. A pointer is a piece of paper with a *street address* written on it — it is not the house, and it doesn't contain the house's contents directly, but if you have that address, you know exactly where to go to find (or change) the house's contents. Critically, the address itself, the piece of paper, is also stored somewhere and has its own location too — a pointer is a real variable in its own right, just one whose job is to hold someone else's address rather than an ordinary value.

## `&` (Address-Of) and `*` (Dereference)

`&variable` produces the address where `variable` is stored — read it as "the address of `variable`." `*pointer` accesses the value stored at the address a pointer holds — read it as "the value pointed to by `pointer`," and this operation is called **dereferencing**.

```c
#include <stdio.h>

int main(void) {
    int age = 30;
    int *age_pointer = &age; // age_pointer holds the address of age

    printf("Value of age: %d\n", age);
    printf("Address of age: %p\n", (void *)&age);
    printf("Value stored in age_pointer: %p\n", (void *)age_pointer);
    printf("Value pointed to by age_pointer: %d\n", *age_pointer);

    return 0;
}
```

### Compile and run

```bash
gcc -std=c11 -Wall -Wextra pointer_basics.c -o pointer_basics
./pointer_basics
```

### Expected output (the actual address will vary every run)

```text
Value of age: 30
Address of age: 0x7ffd3c1a2b3c
Value stored in age_pointer: 0x7ffd3c1a2b3c
Value pointed to by age_pointer: 30
```

Notice "Address of age" and "Value stored in age_pointer" are identical — that is the entire point of the assignment `age_pointer = &age`: `age_pointer` now holds exactly the address where `age` lives, nothing more and nothing less. And `*age_pointer` (dereferencing) retrieves `30`, the value actually stored at that address — the same value as `age` itself, because it *is* the same memory.

### `*` Means Different Things in a Declaration vs. an Expression

This is a very common source of beginner confusion, worth stating explicitly:

- In a **declaration**, `int *age_pointer;` means "`age_pointer` is a variable that will hold the address of an `int`." The `*` here is part of the type, not an operation being performed.
- In an **expression** (anywhere outside a declaration), `*age_pointer` means "dereference `age_pointer`" — go to the address it holds and access the value stored there.

```c
#include <stdio.h>

int main(void) {
    int value = 42;
    int *p; // declaration: p is a pointer to int — * here is part of the type

    p = &value; // assign p the address of value

    *p = 100; // expression: dereference p, then assign 100 to whatever it points to

    printf("value is now: %d\n", value);

    return 0;
}
```

### Compile and run

```bash
gcc -std=c11 -Wall -Wextra dereference_assign.c -o dereference_assign
./dereference_assign
```

### Expected output

```text
value is now: 100
```

`*p = 100;` did not change what `p` points to — `p` still points at `value`, exactly as before. It changed the value *at* the address `p` points to, which is `value`'s own memory — so `value` itself became `100`, even though the assignment statement never mentions `value` by name. This is the mechanism that finally explains how `scanf("%d", &age)` (Chapter 5) is able to write into your variable: `scanf` receives `age`'s address, and internally performs the equivalent of a dereferencing assignment to store the typed-in value directly into `age`'s own memory.

## Memory Addresses Are Just Numbers

An address is, underneath everything, just a number — a position in the computer's memory. `%p` prints it (conventionally in hexadecimal). The specific numeric value you see is essentially meaningless on its own: it depends on decisions made by the operating system and the compiler about where to place this particular program's variables in memory for this particular run, and it will commonly be different the next time you run the very same program.

```c
#include <stdio.h>

int main(void) {
    int x = 5;

    printf("Address of x: %p\n", (void *)&x);

    return 0;
}
```

### Compile and run

```bash
gcc -std=c11 -Wall -Wextra address_is_a_number.c -o address_is_a_number
./address_is_a_number
./address_is_a_number
```

### Expected output (run twice — the two addresses will very likely differ)

```text
Address of x: 0x7ffe5a8c9d4c
Address of x: 0x7ffd1234abcc
```

What matters is never the specific digits of an address, but what having that address *lets you do*: reach the variable it refers to, pass that reach to another function, or step through a sequence of addresses (covered shortly, under pointer arithmetic). Never write code whose correctness depends on a pointer's printed numeric value being any particular thing.

!!! note "Why cast to `(void *)` when printing with `%p`?"
    `%p` technically expects an argument of type `void *` ("pointer to an unspecified type" — a brief preview of `void *`, covered fully in Chapter 17). `&x` has type `int *`, not `void *`, so the explicit cast `(void *)&x` converts it to the type `%p` expects, in a fully portable, warning-free way.

## `NULL` Pointers

`NULL` is a special value (defined in several standard headers, including `<stdio.h>`) representing "this pointer does not point at any valid object." It is good discipline to initialize every pointer either to a real address or explicitly to `NULL`, and to check for `NULL` before dereferencing a pointer that might not have been set to something valid.

```c
#include <stdio.h>

int main(void) {
    int *p = NULL;

    if (p == NULL) {
        printf("p does not point to anything valid yet.\n");
    }

    int value = 7;
    p = &value;

    if (p != NULL) {
        printf("p now points to a value: %d\n", *p);
    }

    return 0;
}
```

### Compile and run

```bash
gcc -std=c11 -Wall -Wextra null_pointer.c -o null_pointer
./null_pointer
```

### Expected output

```text
p does not point to anything valid yet.
p now points to a value: 7
```

!!! danger "Uninitialized pointers are dangerous"
    A pointer variable that is declared but never given a value (`int *p;` with nothing afterward) does not automatically become `NULL` — like any other uninitialized local variable (Chapter 3), it holds an indeterminate, essentially random address left over from whatever was previously in that memory. Dereferencing such a pointer (`*p`) reads or writes to some unpredictable, arbitrary location in memory, which is undefined behavior and a notorious source of crashes and security vulnerabilities. **Always initialize every pointer**, either to a real, valid address or explicitly to `NULL`, and never dereference a pointer you haven't confirmed points somewhere valid.

## Pointers and Arrays

Chapter 9 introduced array decay — that passing an array to a function passes a pointer to its first element — without fully explaining the mechanics. Here is the resolution: **an array name, in most expressions, automatically decays into a pointer to its first element.** This means array indexing and pointer arithmetic are, underneath, the very same operation.

```c
#include <stdio.h>

int main(void) {
    int numbers[5] = {10, 20, 30, 40, 50};
    int *p = numbers; // numbers decays to a pointer to numbers[0]; no & needed here

    for (int i = 0; i < 5; i++) {
        printf("numbers[%d] = %d, *(p + %d) = %d\n", i, numbers[i], i, *(p + i));
    }

    return 0;
}
```

### Compile and run

```bash
gcc -std=c11 -Wall -Wextra pointer_array_relationship.c -o pointer_array_relationship
./pointer_array_relationship
```

### Expected output

```text
numbers[0] = 10, *(p + 0) = 10
numbers[1] = 20, *(p + 1) = 20
numbers[2] = 30, *(p + 2) = 30
numbers[3] = 40, *(p + 3) = 40
numbers[4] = 50, *(p + 4) = 50
```

`numbers[i]` and `*(p + i)` produce exactly the same values at every index, because they are exactly the same operation expressed two different ways: `numbers[i]` is, by definition in C, shorthand for `*(numbers + i)`. This is also why array indexing has always allowed `0`-based indices that "just work" with simple address arithmetic, and why `p` did not need an `&` in front of `numbers` — `numbers` had already decayed into the address of its first element automatically.

## Pointer Arithmetic

When you write `p + 1` for a pointer `p`, C does **not** simply add 1 byte to the address. It advances the address by `sizeof(*p)` bytes — the size of whatever type `p` points to — so that `p + 1` always correctly lands on the *next element* of that type, regardless of how many bytes each element actually occupies.

```c
#include <stdio.h>

int main(void) {
    int numbers[4] = {100, 200, 300, 400};
    int *p = numbers;

    for (int i = 0; i < 4; i++) {
        printf("Address of numbers[%d]: %p\n", i, (void *)(p + i));
    }

    return 0;
}
```

### Compile and run

```bash
gcc -std=c11 -Wall -Wextra pointer_arithmetic.c -o pointer_arithmetic
./pointer_arithmetic
```

### Expected output (addresses vary, but the gaps are consistent)

```text
Address of numbers[0]: 0x7ffd8a3f4c10
Address of numbers[1]: 0x7ffd8a3f4c14
Address of numbers[2]: 0x7ffd8a3f4c18
Address of numbers[3]: 0x7ffd8a3f4c1c
```

Each successive address differs from the previous one by exactly `4` (in hexadecimal, `0x10` to `0x14` to `0x18` to `0x1c`), because `sizeof(int)` is `4` bytes on this system: `p + 1` jumped forward by `4` bytes, not `1` byte, landing precisely on the next `int`. If `p` instead pointed to `double`s (typically 8 bytes each), `p + 1` would jump forward by 8 bytes instead — the compiler automatically scales pointer arithmetic to the size of the pointed-to type, which is exactly what makes `*(p + i)` and `array[i]` behave identically for any element type.

## Passing Pointers to Functions: Finally Fixing the Swap

Chapter 8 showed that a function receiving plain `int` parameters can never modify the caller's original variables, because C passes copies. Now we have the missing piece: pass the variables' *addresses* instead, so the function can dereference those addresses and modify the caller's actual memory directly.

```c
#include <stdio.h>

void swap(int *a, int *b) {
    int temp = *a;
    *a = *b;
    *b = temp;
}

int main(void) {
    int x = 1;
    int y = 2;

    printf("Before swap: x = %d, y = %d\n", x, y);
    swap(&x, &y);
    printf("After swap:  x = %d, y = %d\n", x, y);

    return 0;
}
```

### Compile and run

```bash
gcc -std=c11 -Wall -Wextra swap_fixed.c -o swap_fixed
./swap_fixed
```

### Expected output

```text
Before swap: x = 1, y = 2
After swap:  x = 2, y = 1
```

This time it genuinely works. Walking through why: `swap(&x, &y)` passes the *addresses* of `x` and `y`, not copies of their values. Inside `swap`, `a` and `b` are still themselves copies — copies of the addresses — but an address, once you have it, lets you reach all the way back to the original variable it refers to. `*a = *b;` means "take the value currently at the address `b` holds, and store it at the address `a` holds" — that is a direct write into `main`'s own `y` and `x` memory, not into any copy. This is the resolution promised back in Chapter 8: pass-by-value is still in full effect (the addresses themselves are copied into `a` and `b`), but because what's being copied is an address rather than an ordinary value, dereferencing that address reaches the original data.

!!! tip "The general pattern: \"pass a pointer to let a function modify the original\""
    Any time you want a function to modify a caller's variable, the recipe is the same: the caller passes `&variable`, the function's parameter is declared as a pointer to that type, and the function uses `*parameter = ...` to write through to the original. This is precisely the same mechanism `scanf("%d", &age)` relies on internally, and it is also exactly why passing an array to a function (Chapter 9) lets that function modify the array's actual elements — an array argument is already, via decay, the address of its first element, with no extra `&` required.

## A Brief Teaser: Pointers to Pointers

Just as a pointer can hold the address of an `int`, a pointer can also hold the address of *another pointer*. This is written with two asterisks, `int **pp`, and read as "a pointer to a pointer to `int`."

```c
#include <stdio.h>

int main(void) {
    int value = 42;
    int *p = &value;
    int **pp = &p;

    printf("value: %d\n", value);
    printf("*p:    %d\n", *p);
    printf("**pp:  %d\n", **pp);

    return 0;
}
```

### Compile and run

```bash
gcc -std=c11 -Wall -Wextra pointer_to_pointer.c -o pointer_to_pointer
./pointer_to_pointer
```

### Expected output

```text
value: 42
*p:    42
**pp:  42
```

`pp` holds the address of `p`; dereferencing it once (`*pp`) gives you `p`'s value, which is itself an address (`&value`); dereferencing again (`**pp`) follows that second address to finally reach `value` itself. Each `*` peels back exactly one layer of "address of." Full coverage of pointers-to-pointers, along with function pointers (pointers that point at executable code rather than data) and `void *` ("pointer to an unspecified type," briefly seen earlier with `%p`), is in Chapter 17.

## Common Pitfalls

### Dangling Pointers

A **dangling pointer** points to memory that was once valid but no longer is — for example, the address of a local variable that has already gone out of scope when the function that owned it returned (recall scope from Chapter 8). Using a dangling pointer is undefined behavior, because the memory it refers to may now be reused for something completely unrelated.

```c
#include <stdio.h>

int *get_dangling_pointer(void) {
    int local_value = 99; // local to this function
    return &local_value;  // BUG: returning the address of a variable about to disappear
}

int main(void) {
    int *p = get_dangling_pointer();
    printf("Dereferencing a dangling pointer: %d\n", *p); // undefined behavior

    return 0;
}
```

!!! danger "Do not rely on this example's output"
    `local_value` ceases to exist the moment `get_dangling_pointer` returns, since it was a local variable belonging to that function's call. The address returned in `p` is therefore dangling — it may appear to still "work" by coincidence on some runs (printing `99`), because the memory hasn't been reused yet, while on other runs, or other systems, or after the smallest unrelated change elsewhere in the program, it can print garbage or crash. This program is shown purely to illustrate the mistake, not as a pattern to use or rely on.

### Uninitialized (Wild) Pointers

As warned earlier in this chapter, a pointer that is declared but never assigned holds a meaningless, essentially random address — sometimes called a **wild pointer**. Dereferencing it is just as undefined as dereferencing a dangling pointer, for largely the same reason: you have no guarantee that the address it happens to contain refers to anything valid or anything you're permitted to access.

### Dereferencing `NULL`

```c
#include <stdio.h>

int main(void) {
    int *p = NULL;

    printf("%d\n", *p); // undefined behavior — typically crashes immediately

    return 0;
}
```

!!! danger "Dereferencing `NULL` typically crashes the program"
    Unlike a dangling or wild pointer, which may appear to "work" unpredictably, dereferencing `NULL` on essentially all common modern operating systems triggers an immediate, consistent crash — a **segmentation fault** — because address `0` is deliberately set up by the operating system to never be valid for a normal program to access. This is, in a sense, the "safest" of the pointer mistakes covered here, precisely because it fails loudly and immediately rather than silently corrupting something elsewhere; always check a pointer against `NULL` before dereferencing it if there's any chance it might not have been set to a valid address.

## Summary

- A pointer is a variable that holds a memory address; `&variable` produces that address, and `*pointer` dereferences it, accessing (or modifying) the value actually stored there.
- `*` means something different in a declaration (`int *p;`, part of the type) versus in an expression (`*p`, the dereference operation) — keeping this distinction straight resolves a lot of beginner confusion.
- Addresses are just numbers, printed with `%p`; the specific value is meaningless and varies between runs — what matters is what having the address lets you do.
- `NULL` represents "points at nothing valid"; always initialize pointers to a real address or `NULL`, and check before dereferencing.
- An array name decays to a pointer to its first element; `array[i]` and `*(pointer + i)` are the same operation, resolving Chapter 9's array-decay teaser.
- Pointer arithmetic scales automatically by `sizeof` of the pointed-to type, so `p + 1` lands on the next element, not the next byte.
- Passing `&variable` to a function, with a pointer parameter, finally lets that function modify the caller's own variable through dereferencing — this is how the Chapter 8 failed-swap example is properly fixed.
- Pointers-to-pointers (`int **pp`) chain the same "address of" mechanism one level deeper; full treatment, plus function pointers and `void *`, is in Chapter 17.
- Common pitfalls: dangling pointers (pointing at memory whose owner is gone), wild/uninitialized pointers (meaningless leftover addresses), and dereferencing `NULL` (a typically immediate, loud crash rather than silent corruption).

## Exercises

1. Declare an `int` variable and a pointer to it, then print the variable's value three different ways: directly, through dereferencing the pointer, and by reassigning the variable through the pointer (`*pointer = newValue;`) and printing the variable again.
2. Write a function `void increment(int *n)` that increments the value pointed to by `n`, and call it on a variable in `main`, printing the variable before and after the call to confirm it actually changed.
3. Using the relationship between arrays and pointers, write a loop that prints every element of an array of 6 `double`s using only pointer arithmetic (`*(p + i)`), with no `[]` indexing at all.
4. Write a function `void find_min_max(int arr[], int size, int *min, int *max)` that finds both the minimum and maximum of an array and writes them into the caller's variables via the `min` and `max` pointers (this is a common, genuinely useful pattern for "returning" more than one value from a function, since `return` only allows one).
5. *(Harder)* Deliberately write a small program containing a dangling pointer (similar to the `get_dangling_pointer` example), run it several times, and note in a comment whether the output stayed the same or varied between runs. Then fix it by having the function return the value itself (by value) instead of the address of a local variable, and explain in a comment why that fix is safe.
6. *(Harder)* Write a program with a pointer-to-pointer (`int **pp`) that swaps which of two `int` variables a regular pointer `p` points to (not the values themselves — the pointer's target), by passing `&p` into a function that reassigns `*pp` to point elsewhere. Trace through on paper what each level of indirection refers to before writing the code.

---
[← Chapter 10 — Strings](chapter-10.md) | [Chapter 12 — Dynamic Memory →](chapter-12.md)
