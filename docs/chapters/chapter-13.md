# Chapter 13 — Dynamic Memory Allocation

## Introduction

Every variable you have declared so far — `int x;`, `Point p;`, `int arr[10];` — has its size fixed at compile time, written directly into the source code. But many real problems don't know their size until the program is *running*: how many lines are in a file the user picks, how many records a database query returns, how large a buffer needs to be to hold a network message of unknown length. This chapter introduces **dynamic memory allocation** — requesting memory explicitly while the program runs, from a region called the **heap**, and managing its lifetime yourself.

## Stack vs. Heap

Every variable you have used since Chapter 3 — local variables inside functions — lives on the **stack**. The stack is automatic: when a function is called, space for its local variables is reserved; when the function returns, that space is automatically reclaimed. You never have to ask for it or give it back. The tradeoff is that stack memory's size and lifetime are rigidly tied to the function call that created it: a local array's size must be known at compile time (or, with effort, at the moment the function starts), and the data disappears the instant the function returns — recall from Chapter 8 that returning a pointer to a local variable is unsafe for exactly this reason.

The **heap** is a different region of memory that is *not* tied to any function call. You explicitly request a block of heap memory when you need it, and that memory remains reserved — persisting even after the function that requested it returns — until you explicitly give it back. This manual control is exactly what lets you solve the two problems the stack cannot:

- **Unknown size until runtime** — you can compute exactly how much memory you need (e.g., based on user input or a file's contents) and request precisely that much.
- **Data that must outlive the function that created it** — a heap block survives function returns, so a function can allocate data, return a pointer to it, and the caller can keep using that data safely.

The cost of this flexibility is that the heap has no automatic cleanup: every byte you request, you are responsible for releasing yourself.

## `malloc`: Requesting Memory

`malloc` ("memory allocate"), declared in `<stdlib.h>`, requests a block of memory of a given size in bytes and returns a pointer to the start of that block.

```c
void *malloc(size_t size);
```

It returns `void *` — recall from Chapter 11 that this is a generic pointer type, not tied to any particular data type. In C, a `void *` is automatically converted to any other pointer type without a cast, so you do not need to write `(int *) malloc(...)`. Some C++ programmers (and some C textbooks, for cross-language consistency) prefer to cast explicitly anyway; it is not wrong to do so, but C itself does not require it.

```c
#include <stdio.h>
#include <stdlib.h>

int main(void) {
    int n = 5;
    int *arr = malloc(n * sizeof(int));

    if (arr == NULL) {
        fprintf(stderr, "malloc failed: out of memory\n");
        return 1;
    }

    for (int i = 0; i < n; i++) {
        arr[i] = i * i;
    }

    for (int i = 0; i < n; i++) {
        printf("arr[%d] = %d\n", i, arr[i]);
    }

    free(arr);
    return 0;
}
```

### Compile and run

```bash
gcc -std=c11 -Wall -Wextra mallocbasic.c -o mallocbasic
./mallocbasic
```

### Expected output

```text
arr[0] = 0
arr[1] = 1
arr[2] = 4
arr[3] = 9
arr[4] = 16
```

Two details are essential every time you call `malloc`:

1. **`malloc` does not initialize the memory it returns.** The block may contain leftover, meaningless bytes from whatever previously used that memory ("garbage"). Reading an allocated value before writing to it produces unpredictable results. In the example above, every element is written (`arr[i] = i * i;`) before being read, which is why the output is well-defined.
2. **Always check the return value for `NULL`.** If the system cannot satisfy the request — most commonly because it is out of memory — `malloc` returns `NULL` rather than crashing. A program that uses a `NULL` pointer as if it were valid (e.g., `arr[0] = 1;` when `arr` is `NULL`) invokes undefined behavior, typically crashing immediately. Checking for `NULL` and handling the failure gracefully (as shown above, printing an error and returning early) is not optional defensive style — it is required for a correct program.

`sizeof(int)` is used rather than a hardcoded number like `4`, since `int`'s exact size can vary by platform (recall Chapter 3); always compute allocation sizes in terms of `sizeof`.

## `calloc`: Zero-Initialized Allocation

`calloc` ("clear allocate"), also in `<stdlib.h>`, is similar to `malloc` but differs in two ways: it takes two arguments — a count and a size per element — and it guarantees the returned memory is zero-initialized.

```c
void *calloc(size_t count, size_t size);
```

```c
#include <stdio.h>
#include <stdlib.h>

int main(void) {
    int n = 5;

    int *with_calloc = calloc(n, sizeof(int));
    if (with_calloc == NULL) {
        fprintf(stderr, "calloc failed\n");
        return 1;
    }

    for (int i = 0; i < n; i++) {
        printf("with_calloc[%d] = %d\n", i, with_calloc[i]);
    }

    free(with_calloc);
    return 0;
}
```

### Compile and run

```bash
gcc -std=c11 -Wall -Wextra callocbasic.c -o callocbasic
./callocbasic
```

### Expected output

```text
with_calloc[0] = 0
with_calloc[1] = 0
with_calloc[2] = 0
with_calloc[3] = 0
with_calloc[4] = 0
```

The equivalent using `malloc` requires a manual zeroing loop (or a call to `memset`, not covered here):

```c
int *with_malloc = malloc(n * sizeof(int));
if (with_malloc != NULL) {
    for (int i = 0; i < n; i++) {
        with_malloc[i] = 0;
    }
}
```

`calloc(n, sizeof(int))` is generally preferable to `malloc(n * sizeof(int))` followed by manual zeroing whenever you actually want zeroed memory, both because it is shorter and because some implementations can zero large blocks more efficiently than a manual loop. If you are about to overwrite every element anyway (as in the array-of-squares example above), `malloc` is sufficient and avoids paying for zeroing that would just be immediately overwritten.

## `realloc`: Resizing a Block

`realloc` changes the size of a previously allocated block — growing or shrinking it — while preserving existing data as much as possible.

```c
void *realloc(void *ptr, size_t new_size);
```

If the block grows, the original contents (up to the old size) are preserved; the newly added memory at the end is uninitialized, just like fresh `malloc` memory. If the block shrinks, the contents up to the new, smaller size are preserved. `realloc` may need to move the block to a new memory location entirely to satisfy the request — you should never assume the pointer's address stays the same after a `realloc` call.

### The Defensive Pattern: Always Use a Temporary Pointer

`realloc` can fail, returning `NULL` — and crucially, if it fails, the *original* block is left untouched and still valid. This means you must never write `ptr = realloc(ptr, new_size);` directly: if `realloc` returns `NULL`, you have just overwritten your only pointer to the original block with `NULL`, making that memory permanently unreachable — a memory leak with no way to recover it, since you can no longer call `free` on it either.

The correct, defensive pattern uses a temporary pointer:

```c
#include <stdio.h>
#include <stdlib.h>

int main(void) {
    int n = 3;
    int *arr = malloc(n * sizeof(int));

    if (arr == NULL) {
        fprintf(stderr, "initial malloc failed\n");
        return 1;
    }

    for (int i = 0; i < n; i++) {
        arr[i] = i + 1;
    }

    int new_n = 6;
    int *temp = realloc(arr, new_n * sizeof(int));

    if (temp == NULL) {
        fprintf(stderr, "realloc failed; original data in 'arr' is still valid\n");
        free(arr);
        return 1;
    }

    arr = temp;  /* only reassign after confirming success */

    for (int i = n; i < new_n; i++) {
        arr[i] = i + 1;
    }

    for (int i = 0; i < new_n; i++) {
        printf("arr[%d] = %d\n", i, arr[i]);
    }

    free(arr);
    return 0;
}
```

### Compile and run

```bash
gcc -std=c11 -Wall -Wextra reallocbasic.c -o reallocbasic
./reallocbasic
```

### Expected output

```text
arr[0] = 1
arr[1] = 2
arr[2] = 3
arr[3] = 4
arr[4] = 5
arr[5] = 6
```

If `realloc` fails here, `temp` becomes `NULL` while `arr` still correctly points at the original, valid, unmodified block — so `free(arr)` in the failure branch correctly cleans it up rather than leaking it.

## `free`: Returning Memory

`free`, also in `<stdlib.h>`, returns a previously allocated block to the system so it can be reused by future allocations.

```c
void free(void *ptr);
```

The rule is simple to state and essential to follow: free exactly what you allocated, exactly once. Two specific mistakes violate this rule and are dangerous enough to deserve their own warning.

## Memory Leaks

A **memory leak** happens when allocated memory is never freed, and the program loses every pointer that referred to it — making it impossible to free later, even if you wanted to. The memory remains reserved for the lifetime of the program, unusable by anything else.

```c
#include <stdio.h>
#include <stdlib.h>

void leaky_function(void) {
    int *data = malloc(100 * sizeof(int));
    if (data == NULL) {
        return;
    }
    data[0] = 42;
    /* 'data' goes out of scope here without being freed —
       the 400 bytes it pointed to are now leaked. */
}

int main(void) {
    for (int i = 0; i < 1000; i++) {
        leaky_function();
    }
    printf("Done calling leaky_function 1000 times.\n");
    return 0;
}
```

### Compile and run

```bash
gcc -std=c11 -Wall -Wextra leak.c -o leak
./leak
```

### Expected output

```text
Done calling leaky_function 1000 times.
```

The program runs and prints normally — leaks are not visible in a program's immediate output, which is exactly what makes them dangerous. Each of the 1000 calls leaks 400 bytes (100 `int`s), for 400,000 bytes lost by the time the loop finishes. A long-running program (a server, a daemon) repeating a leak like this can gradually exhaust all available memory, eventually causing allocation failures or a crash, often long after the leaking code ran.

The fix is to free the memory before it goes out of scope:

```c
#include <stdio.h>
#include <stdlib.h>

void non_leaky_function(void) {
    int *data = malloc(100 * sizeof(int));
    if (data == NULL) {
        return;
    }
    data[0] = 42;
    free(data);  /* give the memory back before 'data' goes out of scope */
}

int main(void) {
    for (int i = 0; i < 1000; i++) {
        non_leaky_function();
    }
    printf("Done calling non_leaky_function 1000 times.\n");
    return 0;
}
```

### Compile and run

```bash
gcc -std=c11 -Wall -Wextra noleak.c -o noleak
./noleak
```

### Expected output

```text
Done calling non_leaky_function 1000 times.
```

## Dangling Pointers and Double-Free

!!! danger "Use-after-free and double-free are undefined behavior"
    After `free(ptr)`, the memory `ptr` pointed to is returned to the system and may be reused for something else at any time. `ptr` itself still holds the old address — it becomes a **dangling pointer**, pointing at memory you no longer own.

    - **Use-after-free**: reading or writing through a dangling pointer (e.g., `ptr[0] = 5;` after `free(ptr);`) accesses memory that may now belong to a completely unrelated part of the program, or to nothing at all. This can silently corrupt unrelated data, or crash — and because the result depends on what else has reused that memory, the bug can appear intermittently and be extremely hard to reproduce.
    - **Double-free**: calling `free(ptr)` a second time on a pointer already freed is also undefined behavior. Many allocators maintain internal bookkeeping inside the freed block itself; freeing it twice can corrupt that bookkeeping, leading to crashes or corruption elsewhere that may only surface much later in the program's execution, far from the actual bug.

    Neither mistake is reliably caught by the compiler or guaranteed to crash immediately, which is exactly what makes both so dangerous: the program may appear to work fine for a while before failing unpredictably.

The standard defensive discipline against both problems is simple: **set a pointer to `NULL` immediately after freeing it.**

```c
#include <stdio.h>
#include <stdlib.h>

int main(void) {
    int *p = malloc(sizeof(int));
    if (p == NULL) {
        return 1;
    }

    *p = 10;
    printf("Before free: %d\n", *p);

    free(p);
    p = NULL;  /* defensive: p can no longer be accidentally reused */

    if (p == NULL) {
        printf("p has been freed and is now NULL.\n");
    }

    /* free(p); would now be free(NULL), which is explicitly safe and a no-op */

    return 0;
}
```

### Compile and run

```bash
gcc -std=c11 -Wall -Wextra dangling.c -o dangling
./dangling
```

### Expected output

```text
Before free: 10
p has been freed and is now NULL.
```

Setting `p = NULL;` right after `free(p);` does not prevent every possible misuse (a second, *different* pointer might still refer to the same now-freed block), but it does turn the two most common mistakes — accidentally dereferencing `p` again, or accidentally calling `free(p)` again — into either a clean, detectable `NULL` dereference or a harmless no-op, since `free(NULL)` is explicitly defined by the C standard to do nothing.

## Worked Example: A Growable Array

A common, practical pattern combines everything in this chapter: start with a small heap-allocated array, and grow it with `realloc` as more elements need to be stored than originally fit — a minimal hand-rolled version of the resizable arrays found in many other languages' standard libraries.

```c
#include <stdio.h>
#include <stdlib.h>

int main(void) {
    int capacity = 2;
    int count = 0;
    int *numbers = malloc(capacity * sizeof(int));

    if (numbers == NULL) {
        fprintf(stderr, "initial malloc failed\n");
        return 1;
    }

    int values_to_add[] = {10, 20, 30, 40, 50, 60, 70};
    int total = (int) (sizeof(values_to_add) / sizeof(values_to_add[0]));

    for (int i = 0; i < total; i++) {
        if (count == capacity) {
            int new_capacity = capacity * 2;
            int *temp = realloc(numbers, new_capacity * sizeof(int));

            if (temp == NULL) {
                fprintf(stderr, "realloc failed; aborting\n");
                free(numbers);
                return 1;
            }

            numbers = temp;
            capacity = new_capacity;
            printf("Grew capacity to %d\n", capacity);
        }

        numbers[count] = values_to_add[i];
        count++;
    }

    printf("Final contents (count = %d, capacity = %d):\n", count, capacity);
    for (int i = 0; i < count; i++) {
        printf("numbers[%d] = %d\n", i, numbers[i]);
    }

    free(numbers);
    numbers = NULL;

    return 0;
}
```

### Compile and run

```bash
gcc -std=c11 -Wall -Wextra growable.c -o growable
./growable
```

### Expected output

```text
Grew capacity to 4
Grew capacity to 8
Final contents (count = 7, capacity = 8):
numbers[0] = 10
numbers[1] = 20
numbers[2] = 30
numbers[3] = 40
numbers[4] = 50
numbers[5] = 60
numbers[6] = 70
```

The array starts with room for 2 elements. Each time it fills up (`count == capacity`), the capacity doubles via `realloc`, following the defensive temp-pointer pattern. Doubling, rather than growing by a fixed small amount, is a common strategy that keeps the *total* number of `realloc` calls small even as the array grows very large — though analyzing exactly why is left as a later exercise in algorithmic thinking, beyond this chapter's scope.

## Detecting Memory Errors Automatically

Manually tracking every allocation and free becomes difficult in larger programs. Tools exist that automatically detect leaks, use-after-free, double-free, and other memory errors while a program runs, reporting exactly where the problem occurred. **Valgrind** is the most widely used such tool on Linux; it is covered in Chapter 20 — Debugging and Best Practices.

## Summary

- The stack holds local variables automatically, with a lifetime tied to the function call that created them; the heap holds memory you request and release manually, with a lifetime you control.
- `malloc(size)` requests `size` bytes and returns an uninitialized block as `void *`, which converts implicitly to any pointer type in C; always check its return for `NULL`.
- `calloc(count, size)` requests zero-initialized memory for `count` elements of `size` bytes each, avoiding a manual zeroing loop.
- `realloc(ptr, new_size)` resizes a previously allocated block, preserving existing contents; always assign its result to a temporary pointer first, and only overwrite the original pointer after confirming the result is not `NULL`, to avoid leaking the original block on failure.
- `free(ptr)` returns memory to the system; you must free exactly what you allocated, exactly once.
- A memory leak occurs when allocated memory's last pointer is lost without freeing it first; leaks are invisible in normal output but can exhaust memory over time.
- Use-after-free and double-free are both undefined behavior; setting a pointer to `NULL` immediately after `free` is a cheap, effective defensive habit, since `free(NULL)` is explicitly safe.
- A growable array — `malloc` a small block, `realloc` to grow it as needed, `free` when done — is a practical pattern combining all of this chapter's tools.
- Valgrind and similar tools can automatically detect leaks and memory errors; covered in Chapter 20 — Debugging and Best Practices.

## Exercises

1. Write a program that asks the user how many integers they want to enter, `malloc`s an array of exactly that size, reads that many integers with `scanf` (recall Chapter 5), prints their sum, and frees the array.
2. Rewrite exercise 1 to use `calloc` instead of `malloc`, and add a print statement before reading any input that shows all elements are already zero.
3. Write a program with a function `int *make_squares(int n)` that `malloc`s an array of `n` ints, fills it with the squares `0, 1, 4, 9, ...`, and returns the pointer (this is a case where heap memory's ability to outlive the function that created it is essential — explain why in a comment). Call it from `main`, print the results, and free the array there.
4. Take the leaking program from this chapter (`leak.c`) and modify it to leak a *variable* amount of memory each call (e.g., based on a loop counter), then fix it. Add a comment explaining specifically where the only pointer to each allocated block was lost in the original.
5. *(Harder)* Extend the growable-array worked example into a small set of three functions: `int *array_create(int initial_capacity)`, `void array_append(int **arr, int *count, int *capacity, int value)` (taking pointers to the caller's count and capacity so it can update them), and `void array_destroy(int **arr)` (which frees and sets the caller's pointer to `NULL`). Use them from `main` to build an array of 20 values.
6. *(Harder)* Write a program that intentionally demonstrates (in a controlled, commented way) the danger of reassigning a pointer directly from `realloc` without a temporary — you cannot easily force `realloc` to fail on demand, so instead write the *correct* defensive version, then write a second version with the bug (`arr = realloc(arr, ...)` directly) and explain in comments, step by step, exactly what would be lost if that call had returned `NULL`.

---
[← Chapter 12 — Structures, Unions and Enums](chapter-12.md) | [Chapter 14 — File Handling →](chapter-14.md)
