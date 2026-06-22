# Chapter 17 — Advanced Pointers

## Introduction

Chapters 10 and 11 covered the fundamentals of pointers: the address-of operator `&`, the dereference operator `*`, `NULL`, pointer arithmetic on arrays, and passing pointers to functions so they can modify a caller's data. Chapter 11 also left you with a teaser: a glimpse of `int **pp`, a pointer to a pointer. This chapter resolves that teaser fully, and then pushes pointers further than most introductory material does: function pointers, generic `void *` pointers, `const` correctness, a deeper look at pointer arithmetic, and dynamically built 2D structures. These are the tools that show up constantly in real C codebases — callback-driven APIs, generic data structures, and hand-rolled matrices — so mastering them here pays off for the rest of the tutorial.

## Pointer-to-Pointer: Resolving the Teaser

A pointer variable holds the address of something. An ordinary `int *p` holds the address of an `int`. A pointer-to-pointer, `int **pp`, holds the address of an `int *` — that is, the address of a variable that itself stores an address.

```c
#include <stdio.h>

int main(void) {
    int value = 42;
    int *p = &value;     /* p holds the address of value */
    int **pp = &p;       /* pp holds the address of p */

    printf("value          = %d\n", value);
    printf("*p             = %d\n", *p);
    printf("**pp           = %d\n", **pp);
    printf("p  (address)   = %p\n", (void *)p);
    printf("*pp (address)  = %p\n", (void *)*pp);
    printf("&value         = %p\n", (void *)&value);

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
value          = 42
*p             = 42
**pp           = 42
p  (address)   = 0x7ffd3c2a4a1c
*pp (address)  = 0x7ffd3c2a4a1c
&value         = 0x7ffd3c2a4a1c
```

(The actual addresses will differ on your machine and on every run — what matters is that `p`, `*pp`, and `&value` are all the same address.)

Reading `int **pp` right to left from the name outward: `pp` is a pointer to (a pointer to (an `int`)). Dereferencing once, `*pp`, gives you back an `int *` — that is, `p`'s value. Dereferencing twice, `**pp`, gives you the `int` itself.

### When you actually need a pointer-to-pointer

A pointer-to-pointer is not just a curiosity; it solves a real, recurring problem: **a function cannot modify a caller's pointer variable unless it receives the address of that pointer variable.** Recall from Chapter 11 that passing a plain pointer lets a function modify the *data the pointer points to*, but the pointer variable itself is passed by value — the function gets its own local copy of the address, and changes to that copy are not visible to the caller.

This matters whenever a function needs to make the caller's pointer point somewhere new — most commonly, reallocating a buffer the caller owns.

```c
#include <stdio.h>
#include <stdlib.h>

/* grow_buffer needs to change the caller's pointer itself (in case
 * realloc moves the block), so it must take the ADDRESS of that
 * pointer: a pointer-to-pointer. */
int grow_buffer(int **buffer, size_t *capacity, size_t new_capacity) {
    int *new_block = realloc(*buffer, new_capacity * sizeof(int));
    if (new_block == NULL) {
        return 0; /* realloc failed; caller's buffer is untouched */
    }
    *buffer = new_block;     /* update the CALLER's pointer */
    *capacity = new_capacity;
    return 1;
}

int main(void) {
    size_t capacity = 4;
    int *numbers = malloc(capacity * sizeof(int));
    if (numbers == NULL) {
        fprintf(stderr, "initial allocation failed\n");
        return 1;
    }

    for (size_t i = 0; i < capacity; i++) {
        numbers[i] = (int)i * 10;
    }

    printf("Before growth, capacity = %zu\n", capacity);

    if (!grow_buffer(&numbers, &capacity, 8)) {
        fprintf(stderr, "grow_buffer failed\n");
        free(numbers);
        return 1;
    }

    printf("After growth, capacity = %zu\n", capacity);
    for (size_t i = 0; i < capacity; i++) {
        if (i >= 4) {
            numbers[i] = (int)i * 100; /* fill the newly grown space */
        }
        printf("numbers[%zu] = %d\n", i, numbers[i]);
    }

    free(numbers);
    return 0;
}
```

### Compile and run

```bash
gcc -std=c11 -Wall -Wextra grow_buffer.c -o grow_buffer
./grow_buffer
```

### Expected output

```text
Before growth, capacity = 4
After growth, capacity = 8
numbers[0] = 0
numbers[1] = 10
numbers[2] = 20
numbers[3] = 30
numbers[4] = 400
numbers[5] = 500
numbers[6] = 600
numbers[7] = 700
```

Without the `int **buffer` parameter, `grow_buffer` could reallocate a *local copy* of the pointer, but `main`'s `numbers` would still point at the old (possibly freed, possibly relocated) block — a recipe for the dangling-pointer bugs covered in Chapter 13.

The other classic use of `int **` is representing a dynamically built 2D array as an array of row pointers, which this chapter covers in detail later on.

!!! note "Levels of indirection"
    There is nothing stopping you from going further — `int ***ppp` is legal — but pointer-to-pointer is by far the most common case in practice. If you find yourself reaching for three or more levels of indirection, it is worth stepping back and asking whether a clearer data structure (perhaps a struct, covered in Chapter 12) would express the same idea more simply.

## Function Pointers

Functions, like data, live at addresses in memory. C lets you store a function's address in a variable and call the function indirectly through that variable. This enables dispatch tables, callbacks, and plugin-style designs.

### Declaration syntax

```text
int (*fp)(int, int);
```

Read this as: `fp` is a pointer to a function that takes two `int` parameters and returns `int`. The parentheses around `*fp` are required — without them, `int *fp(int, int)` would declare a function named `fp` that *returns* `int *`, which is a completely different thing.

### Assigning and calling through a function pointer

```c
#include <stdio.h>

int add(int a, int b) {
    return a + b;
}

int multiply(int a, int b) {
    return a * b;
}

int main(void) {
    int (*fp)(int, int);

    fp = add;                 /* function name decays to its address */
    printf("fp = add:      %d\n", fp(3, 4));      /* call through fp */
    printf("fp = add:      %d\n", (*fp)(3, 4));   /* equivalent, explicit form */

    fp = multiply;
    printf("fp = multiply: %d\n", fp(3, 4));

    return 0;
}
```

### Compile and run

```bash
gcc -std=c11 -Wall -Wextra function_pointer_basics.c -o function_pointer_basics
./function_pointer_basics
```

### Expected output

```text
fp = add:      7
fp = add:      7
fp = multiply: 12
```

Both `fp(3, 4)` and `(*fp)(3, 4)` call the function — C allows the shorter form for convenience, but the explicit `(*fp)(...)` form makes clear what is actually happening: the pointer is dereferenced to reach the function, which is then called.

### Dispatch table: an array of function pointers

A dispatch table maps a key (here, an operator character) to a function pointer, replacing what would otherwise be a `switch` or chain of `if`-`else` statements.

```c
#include <stdio.h>

int add(int a, int b)      { return a + b; }
int subtract(int a, int b) { return a - b; }
int multiply(int a, int b) { return a * b; }
int divide(int a, int b) {
    if (b == 0) {
        printf("Error: division by zero\n");
        return 0;
    }
    return a / b;
}

typedef struct {
    char operator;
    int (*func)(int, int);
} Operation;

int main(void) {
    Operation table[] = {
        { '+', add },
        { '-', subtract },
        { '*', multiply },
        { '/', divide },
    };
    size_t table_size = sizeof(table) / sizeof(table[0]);

    char ops_to_try[] = { '+', '-', '*', '/', '%' };
    int a = 10, b = 3;

    for (size_t t = 0; t < sizeof(ops_to_try); t++) {
        char op = ops_to_try[t];
        int found = 0;

        for (size_t i = 0; i < table_size; i++) {
            if (table[i].operator == op) {
                printf("%d %c %d = %d\n", a, op, b, table[i].func(a, b));
                found = 1;
                break;
            }
        }
        if (!found) {
            printf("Unknown operator: %c\n", op);
        }
    }

    return 0;
}
```

### Compile and run

```bash
gcc -std=c11 -Wall -Wextra dispatch_table.c -o dispatch_table
./dispatch_table
```

### Expected output

```text
10 + 3 = 13
10 - 3 = 7
10 * 3 = 30
10 / 3 = 3
Unknown operator: %
```

### Function pointers as callbacks

A callback is a function pointer passed into another function, which calls it back at the appropriate moment. This is how you write a single generic helper that applies any operation to every element of an array, without that helper needing to know what the operation actually does.

```c
#include <stdio.h>

void apply_to_all(int *array, size_t length, void (*operation)(int *)) {
    for (size_t i = 0; i < length; i++) {
        operation(&array[i]);   /* call back into caller-supplied logic */
    }
}

void double_value(int *value) {
    *value *= 2;
}

void square_value(int *value) {
    *value *= *value;
}

void print_value(int *value) {
    printf("%d ", *value);
}

int main(void) {
    int numbers[] = { 1, 2, 3, 4, 5 };
    size_t length = sizeof(numbers) / sizeof(numbers[0]);

    printf("Original: ");
    apply_to_all(numbers, length, print_value);
    printf("\n");

    apply_to_all(numbers, length, double_value);
    printf("Doubled:  ");
    apply_to_all(numbers, length, print_value);
    printf("\n");

    apply_to_all(numbers, length, square_value);
    printf("Squared:  ");
    apply_to_all(numbers, length, print_value);
    printf("\n");

    return 0;
}
```

### Compile and run

```bash
gcc -std=c11 -Wall -Wextra apply_to_all.c -o apply_to_all
./apply_to_all
```

### Expected output

```text
Original: 1 2 3 4 5 
Doubled:  2 4 6 8 10 
Squared:  4 16 36 64 100 
```

`apply_to_all` never needs to be modified to support a new operation — you just write a new small function matching the `void (*)(int *)` signature and pass it in. This is the same principle the standard library uses for sorting, which leads directly into `void *`.

## `void *`: Generic Pointers

A `void *` is a pointer with no associated type — it stores an address but says nothing about what kind of data lives there. This makes `void *` the tool of choice for writing generic code that works with data of any type, at the cost of giving up compile-time type checking on that pointer.

You cannot dereference a `void *` directly (`*ptr` is meaningless when the compiler doesn't know the size or layout of what `ptr` points to). You must first cast it to a concrete pointer type.

```c
#include <stdio.h>

int main(void) {
    int x = 100;
    void *generic = &x;   /* legal: any object pointer converts to void * */

    /* *generic = 5;       would NOT compile: cannot dereference void * */

    int *typed = (int *)generic;   /* cast back to a concrete type */
    *typed = 5;                     /* now dereferencing is well-defined */

    printf("x = %d\n", x);
    return 0;
}
```

### Compile and run

```bash
gcc -std=c11 -Wall -Wextra void_pointer_basics.c -o void_pointer_basics
./void_pointer_basics
```

### Expected output

```text
x = 5
```

### A practical generic function: `swap_bytes`

Because `void *` carries no size information, generic functions that operate on raw memory almost always need an explicit `size_t size` parameter alongside the `void *`, so they know how many bytes to touch. `memcpy`, introduced in Chapter 13, is the natural tool for moving those bytes around.

```c
#include <stdio.h>
#include <string.h>

void swap_bytes(void *a, void *b, size_t size) {
    unsigned char temp[64]; /* scratch buffer; assumes size <= 64 here */

    if (size > sizeof(temp)) {
        fprintf(stderr, "swap_bytes: size too large for scratch buffer\n");
        return;
    }

    memcpy(temp, a, size);
    memcpy(a, b, size);
    memcpy(b, temp, size);
}

struct Point { int x, y; };

int main(void) {
    int i1 = 1, i2 = 2;
    swap_bytes(&i1, &i2, sizeof(int));
    printf("i1 = %d, i2 = %d\n", i1, i2);

    double d1 = 1.5, d2 = 2.5;
    swap_bytes(&d1, &d2, sizeof(double));
    printf("d1 = %.1f, d2 = %.1f\n", d1, d2);

    struct Point p1 = { 1, 2 };
    struct Point p2 = { 9, 9 };
    swap_bytes(&p1, &p2, sizeof(struct Point));
    printf("p1 = (%d, %d), p2 = (%d, %d)\n", p1.x, p1.y, p2.x, p2.y);

    return 0;
}
```

### Compile and run

```bash
gcc -std=c11 -Wall -Wextra swap_bytes.c -o swap_bytes
./swap_bytes
```

### Expected output

```text
i1 = 2, i2 = 1
d1 = 2.5, d2 = 1.5
p1 = (9, 9), p2 = (1, 2)
```

`swap_bytes` doesn't know or care whether it's swapping `int`s, `double`s, or `struct Point` values — it just moves `size` bytes around. This is exactly the kind of type-erasure trick `void *` exists for.

!!! warning "Using a fixed-size scratch buffer"
    The `unsigned char temp[64]` above is a simplification to keep the example self-contained. A production-quality version would either reject oversized requests more gracefully or allocate the scratch space dynamically with `malloc`/`free` (Chapter 13) sized exactly to `size`.

### `qsort`: the standard library's `void *` + function pointer combo

The standard library's `qsort` function (declared in `<stdlib.h>`) is the canonical real-world example of function pointers and `void *` working together. Its signature is:

```text
void qsort(void *base, size_t nmemb, size_t size,
           int (*compar)(const void *, const void *));
```

`qsort` knows nothing about the type of the elements it is sorting. It treats the array as a raw block of `nmemb` elements, each `size` bytes, and calls your comparator — a function pointer you supply — every time it needs to know the relative order of two elements. Your comparator receives two `void *` parameters, casts them to the real type, and returns negative, zero, or positive following the same convention as `strcmp` (Chapter 13).

```c
#include <stdio.h>
#include <stdlib.h>

int compare_ints_ascending(const void *a, const void *b) {
    int x = *(const int *)a;
    int y = *(const int *)b;
    return (x > y) - (x < y); /* avoids overflow vs. a simple subtraction */
}

int main(void) {
    int numbers[] = { 42, 7, 19, -3, 88, 0, 15 };
    size_t count = sizeof(numbers) / sizeof(numbers[0]);

    qsort(numbers, count, sizeof(int), compare_ints_ascending);

    printf("Sorted: ");
    for (size_t i = 0; i < count; i++) {
        printf("%d ", numbers[i]);
    }
    printf("\n");

    return 0;
}
```

### Compile and run

```bash
gcc -std=c11 -Wall -Wextra qsort_example.c -o qsort_example
./qsort_example
```

### Expected output

```text
Sorted: -3 0 7 15 19 42 88 
```

`qsort` is a direct generalization of the dispatch-table and callback ideas above: instead of hardcoding "ascending integer order," it lets you supply any ordering for any element type, as long as you give it the element size and a conforming comparator.

## `const` Correctness with Pointers

`const` placement around a pointer declaration controls two independent things: whether the *pointed-to data* can be modified through that pointer, and whether the *pointer itself* can be repointed. The cleanest way to parse these declarations is **right to left, starting at the variable name.**

```c
#include <stdio.h>

int main(void) {
    int x = 10;
    int y = 20;

    /* 1. pointer to const data: read "p1 is a pointer to a const int" */
    const int *p1 = &x;
    /* *p1 = 99;       ERROR: cannot modify *p1 */
    p1 = &y;            /* OK: repointing p1 itself is allowed */
    printf("p1 -> %d\n", *p1);

    /* 2. const pointer (to non-const data): "p2 is a const pointer to an int" */
    int *const p2 = &x;
    *p2 = 99;           /* OK: modifying the pointed-to int is allowed */
    /* p2 = &y;         ERROR: cannot repoint a const pointer */
    printf("p2 -> %d\n", *p2);

    /* 3. const pointer to const data: combines both restrictions */
    const int *const p3 = &x;
    /* *p3 = 1;         ERROR: cannot modify *p3 */
    /* p3 = &y;         ERROR: cannot repoint p3 */
    printf("p3 -> %d\n", *p3);

    return 0;
}
```

### Compile and run

```bash
gcc -std=c11 -Wall -Wextra const_correctness.c -o const_correctness
./const_correctness
```

### Expected output

```text
p1 -> 20
p2 -> 99
p3 -> 99
```

| Declaration | Read right-to-left | Can modify `*p`? | Can repoint `p`? |
|---|---|---|---|
| `const int *p` | `p` is a pointer to `const int` | No | Yes |
| `int *const p` | `p` is a `const` pointer to `int` | Yes | No |
| `const int *const p` | `p` is a `const` pointer to `const int` | No | No |

!!! tip "Why this matters in function signatures"
    Marking a pointer parameter `const` (e.g. `void print_array(const int *array, size_t length)`) is a contract with every caller: this function promises not to modify the data you pass in. It costs nothing at runtime and the compiler enforces it for you — use it on every pointer parameter that a function only reads.

## Pointer Arithmetic Deep Dive

Chapter 11 introduced pointer arithmetic: incrementing a pointer moves it by `sizeof` one element of its type, not by one byte, and array indexing (`a[i]`) is defined in terms of pointer arithmetic (`*(a + i)`). This section extends that foundation with subtraction, comparison, and a precise statement of how arrays and pointers differ.

### Pointer subtraction

Subtracting two pointers that point into the *same array* yields the number of elements between them — not the number of bytes.

```c
#include <stdio.h>

int main(void) {
    int values[] = { 10, 20, 30, 40, 50 };
    int *start = &values[0];
    int *end = &values[4];

    ptrdiff_t distance = end - start; /* ptrdiff_t: from <stddef.h>, via <stdio.h> on most systems */
    printf("Distance from start to end: %ld elements\n", (long)distance);
    printf("Byte distance would be:     %ld bytes\n",
           (long)distance * (long)sizeof(int));

    return 0;
}
```

### Compile and run

```bash
gcc -std=c11 -Wall -Wextra pointer_subtraction.c -o pointer_subtraction
./pointer_subtraction
```

### Expected output

```text
Distance from start to end: 4 elements
Byte distance would be:     16 bytes
```

`ptrdiff_t` is the signed integer type the standard guarantees is wide enough to hold the result of pointer subtraction. Subtracting pointers into *different* arrays (or unrelated objects) is undefined behavior — the result is meaningless and must never be relied upon.

### Comparing pointers

Pointers into the same array can be compared with `<`, `<=`, `>`, `>=`, and `==`, and the comparison reflects their relative position within the array.

```c
#include <stdio.h>

int main(void) {
    int values[] = { 5, 15, 25, 35 };
    int *p = &values[1];
    int *q = &values[3];

    if (p < q) {
        printf("p comes before q in the array\n");
    }

    int *r = &values[1];
    if (p == r) {
        printf("p and r point to the same element\n");
    }

    return 0;
}
```

### Compile and run

```bash
gcc -std=c11 -Wall -Wextra pointer_comparison.c -o pointer_comparison
./pointer_comparison
```

### Expected output

```text
p comes before q in the array
p and r point to the same element
```

### Arrays are not pointer variables

Chapter 11 showed that an array "decays" to a pointer to its first element in most expressions. It is essential to understand the limit of that statement: **the array variable itself is not a pointer variable, and it cannot be reassigned.**

```c
#include <stdio.h>

int main(void) {
    int data[3] = { 1, 2, 3 };
    int other[3] = { 4, 5, 6 };

    int *p = data;     /* fine: data decays to &data[0] for this assignment */
    p = other;          /* also fine: p is a real pointer variable, it can be repointed */

    printf("p now points at other[0] = %d\n", *p);

    /* data = other;   ERROR: array type is not assignable; data is not a pointer variable */

    printf("sizeof(data)  = %zu (the whole array, 3 ints)\n", sizeof(data));
    printf("sizeof(p)     = %zu (just one pointer)\n", sizeof(p));

    return 0;
}
```

### Compile and run

```bash
gcc -std=c11 -Wall -Wextra array_vs_pointer.c -o array_vs_pointer
./array_vs_pointer
```

### Expected output

```text
p now points at other[0] = 4
sizeof(data)  = 12 (the whole array, 3 ints)
sizeof(p)     = 8 (just one pointer)
```

The commented-out `data = other;` line is the key takeaway: `data` is a block of storage with a fixed identity and size, known at compile time. It decays to a pointer value *in expressions* (such as when passed to a function or assigned to a pointer variable), but `data` the variable can never itself be made to point somewhere else, unlike `p`. This is also why `sizeof` an array gives the size of the whole array, while `sizeof` a pointer always gives the size of just the pointer (typically 8 bytes on a 64-bit platform).

## Dynamically Allocated 2D-Like Structures

C arrays are not natively multidimensional in the way some languages' are; a true 2D array (`int grid[ROWS][COLS]`) has a fixed compile-time size. To build a 2D structure whose dimensions are decided at runtime, the two most common patterns are an **array of pointers** (a "row pointer" array) and a **flat block with manual indexing**.

### Pattern 1: array of pointers

```c
#include <stdio.h>
#include <stdlib.h>

int main(void) {
    int rows = 3;
    int cols = 4;

    /* Step 1: allocate an array of row pointers */
    int **grid = malloc((size_t)rows * sizeof(int *));
    if (grid == NULL) {
        fprintf(stderr, "allocation failed\n");
        return 1;
    }

    /* Step 2: allocate each row separately */
    for (int i = 0; i < rows; i++) {
        grid[i] = malloc((size_t)cols * sizeof(int));
        if (grid[i] == NULL) {
            fprintf(stderr, "allocation failed for row %d\n", i);
            /* free everything allocated so far before exiting */
            for (int j = 0; j < i; j++) {
                free(grid[j]);
            }
            free(grid);
            return 1;
        }
    }

    /* Fill and print using familiar grid[i][j] syntax */
    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) {
            grid[i][j] = i * cols + j;
        }
    }

    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) {
            printf("%3d ", grid[i][j]);
        }
        printf("\n");
    }

    /* Step 3: free in the REVERSE order of allocation — each row first,
     * then the array of row pointers itself. Recall from Chapter 13 that
     * freeing grid before its rows would leak every row's memory. */
    for (int i = 0; i < rows; i++) {
        free(grid[i]);
    }
    free(grid);

    return 0;
}
```

### Compile and run

```bash
gcc -std=c11 -Wall -Wextra array_of_pointers_grid.c -o array_of_pointers_grid
./array_of_pointers_grid
```

### Expected output

```text
  0   1   2   3 
  4   5   6   7 
  8   9  10  11 
```

This pattern's appeal is the natural `grid[i][j]` syntax, identical to a compile-time 2D array. Its cost is `rows + 1` separate allocations, each with its own bookkeeping overhead and its own opportunity to fail or leak, and the rows are not guaranteed to be contiguous in memory.

### Pattern 2: one flat block with manual indexing

```c
#include <stdio.h>
#include <stdlib.h>

int main(void) {
    int rows = 3;
    int cols = 4;

    /* A single allocation for all rows*cols elements */
    int *grid = malloc((size_t)rows * (size_t)cols * sizeof(int));
    if (grid == NULL) {
        fprintf(stderr, "allocation failed\n");
        return 1;
    }

    /* Manual 2D-to-1D index translation: row i, column j -> i*cols + j */
    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) {
            grid[i * cols + j] = i * cols + j;
        }
    }

    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) {
            printf("%3d ", grid[i * cols + j]);
        }
        printf("\n");
    }

    free(grid); /* exactly one allocation to free */

    return 0;
}
```

### Compile and run

```bash
gcc -std=c11 -Wall -Wextra flat_block_grid.c -o flat_block_grid
./flat_block_grid
```

### Expected output

```text
  0   1   2   3 
  4   5   6   7 
  8   9  10  11 
```

| | Array of pointers | Flat block |
|---|---|---|
| Allocations | `rows + 1` | `1` |
| Syntax | `grid[i][j]` (natural) | `grid[i * cols + j]` (manual) |
| Memory layout | Rows may be scattered | Always contiguous |
| Freeing | Nested loop, reverse order | Single `free` |
| Typical use | Jagged/irregular row lengths | Fixed-shape matrices, performance-sensitive code |

Use the array-of-pointers pattern when row lengths genuinely differ (a jagged array) or when the natural `grid[i][j]` syntax is worth the bookkeeping. Use the flat-block pattern for true rectangular matrices, especially in performance-sensitive code, since one contiguous allocation is cache-friendlier and requires only one `malloc`/`free` pair.

## Summary

- A pointer-to-pointer (`int **pp`) stores the address of a pointer variable. It is required whenever a function must modify the caller's pointer itself — most commonly when reallocating a buffer the caller owns, or when representing a dynamic 2D array as an array of row pointers.
- A function pointer (`int (*fp)(int, int)`) stores the address of a function, and can be called through directly. Arrays of function pointers build dispatch tables; passing a function pointer as a parameter implements a callback.
- `void *` is a generic, typeless pointer. It cannot be dereferenced directly — cast it to a concrete type first. The standard library's `qsort` combines a `void *` array with a function-pointer comparator to sort any data type generically.
- `const` placement around a pointer is read right-to-left: `const int *p` is a pointer to const data (repointable, not modifiable through it); `int *const p` is a const pointer to non-const data (modifiable through it, not repointable); `const int *const p` combines both restrictions.
- Pointer subtraction between two pointers into the same array yields a count of elements (type `ptrdiff_t`), not bytes; pointer comparison reflects relative position within the same array; both are undefined behavior across unrelated objects.
- An array variable is not itself a pointer variable — it cannot be reassigned — even though it decays to a pointer to its first element in most expressions.
- Dynamic 2D structures can be built as an array of separately allocated row pointers (`int **grid`, natural `grid[i][j]` syntax, multiple allocations) or as one flat block with manual index arithmetic (`grid[i*cols+j]`, a single allocation, guaranteed contiguity).

## Exercises

1. Write a function `void increment_through_pointer(int *p)` and a function `void repoint(int **pp, int *new_target)` that demonstrates the difference between modifying data through a pointer versus modifying the pointer variable itself.
2. Write a dispatch table (array of function pointers) for at least four simple string-transformation functions (e.g. uppercase, lowercase, reverse, count vowels) selected by an integer menu choice.
3. Write a generic `void print_array(const void *array, size_t count, size_t elem_size, void (*print_one)(const void *))` that prints any array type by calling a type-specific `print_one` callback for each element; demonstrate it on an array of `int` and an array of `double`.
4. Using `qsort`, sort an array of `struct Employee { char name[32]; int salary; }` first by salary ascending, then write a second comparator and re-sort by name using `strcmp`.
5. *(Harder)* Implement a dynamically growable 2D grid using the flat-block pattern, with a function `int *grid_at(int *grid, int cols, int row, int col)` that returns a pointer to the requested cell, and a `resize_grid` function that uses an `int **` parameter to safely grow the underlying allocation in place.
6. *(Harder)* Write a small program demonstrating that comparing or subtracting pointers from two *different*, unrelated arrays produces meaningless results — allocate two separate arrays, print the "distance" your compiler computes between pointers into each, and explain in a comment why this number cannot be trusted, tying the explanation back to the undefined behavior concept.

---
[← Chapter 16 — Recursion and the Call Stack](chapter-16.md) | [Chapter 18 — Data Structures in C →](chapter-18.md)
