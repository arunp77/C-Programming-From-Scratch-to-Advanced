# Chapter 9 — Arrays

## Introduction

Every variable we've used so far holds exactly one value. Many real problems need to work with a whole collection of related values at once — a list of exam scores, the pixels of an image, the days of the week. An **array** is C's basic tool for this: a fixed-size, contiguous block of memory holding multiple values of the same type, accessed by position. This chapter covers declaring, initializing, and using arrays, the practical pattern for passing them to functions, and several classic array algorithms.

## What an Array Is

An array is a sequence of elements, all of the same type, stored back-to-back in memory with no gaps between them ("contiguous"). Its size — the number of elements it holds — is fixed once it's declared and cannot grow or shrink. Each element is accessed by an **index**, an integer position starting at `0` for the first element, `1` for the second, and so on up to `size - 1` for the last — this is called **0-based indexing**, and it is consistent throughout C.

```c
#include <stdio.h>

int main(void) {
    int scores[5]; // an array of 5 ints, indices 0 through 4

    scores[0] = 91;
    scores[1] = 85;
    scores[2] = 77;
    scores[3] = 100;
    scores[4] = 63;

    printf("First score: %d\n", scores[0]);
    printf("Last score: %d\n", scores[4]);

    return 0;
}
```

### Compile and run

```bash
gcc -std=c11 -Wall -Wextra array_basic.c -o array_basic
./array_basic
```

### Expected output

```text
First score: 91
Last score: 63
```

`scores[5]` declares room for exactly 5 `int` values. There is no `scores[5]` element — the valid indices are only `0`, `1`, `2`, `3`, and `4`. This off-by-one point is the single most common source of array bugs for beginners, and is revisited shortly.

## Initialization

An array can be given initial values at the point of declaration, using a brace-enclosed list:

```c
#include <stdio.h>

int main(void) {
    int full[5] = {10, 20, 30, 40, 50};
    int partial[5] = {1, 2}; // remaining elements (index 2, 3, 4) default to 0
    int zeroed[5] = {0};    // every element explicitly set to 0

    for (int i = 0; i < 5; i++) {
        printf("full[%d] = %d, partial[%d] = %d, zeroed[%d] = %d\n",
               i, full[i], i, partial[i], i, zeroed[i]);
    }

    return 0;
}
```

### Compile and run

```bash
gcc -std=c11 -Wall -Wextra array_init.c -o array_init
./array_init
```

### Expected output

```text
full[0] = 10, full[1] = 20, full[2] = 30, full[3] = 40, full[4] = 50
partial[0] = 1, partial[1] = 2, partial[2] = 0, partial[3] = 0, partial[4] = 0
zeroed[0] = 0, zeroed[1] = 0, zeroed[2] = 0, zeroed[3] = 0, zeroed[4] = 0
```

When an initializer list supplies fewer values than the array's declared size, C automatically sets every remaining element to `0` — it does **not** leave them with indeterminate leftover values the way an uninitialized plain variable would (recall indeterminate values from Chapter 3). This is why `{0}` is a common, reliable idiom for "give me an array that's entirely zeroed out," even for arrays much larger than one element.

!!! tip "Let the compiler count for you"
    If you supply a full initializer list, you can omit the size and let the compiler count the elements: `int full[] = {10, 20, 30, 40, 50};` declares an array of exactly 5 elements. This avoids a whole class of bug where the stated size and the actual initializer list silently disagree.

## Accessing and Modifying Elements

Array elements are read and written using `array[index]`, which can appear on either side of an assignment, exactly like an ordinary variable:

```c
#include <stdio.h>

int main(void) {
    int values[4] = {5, 10, 15, 20};

    values[2] = values[0] + values[1]; // overwrite index 2 using indices 0 and 1

    for (int i = 0; i < 4; i++) {
        printf("values[%d] = %d\n", i, values[i]);
    }

    return 0;
}
```

### Compile and run

```bash
gcc -std=c11 -Wall -Wextra array_modify.c -o array_modify
./array_modify
```

### Expected output

```text
values[0] = 5
values[1] = 10
values[2] = 15
values[3] = 20
```

Note `values[2]` was `15` before this assignment too, purely by coincidence of the chosen numbers (`5 + 10 = 15`) — the point is that `values[2] = values[0] + values[1];` genuinely reads two elements and writes a third, just as ordinary variables would in any other expression.

## Array Bounds: There Is No Automatic Checking

C does **not** check, at the time you write `array[index]`, whether `index` is actually a valid position within the array. Reading or writing past either end of an array is **undefined behavior** — the program might crash, might silently corrupt unrelated data, or might appear to "work" by accident, depending on what happens to be in memory nearby.

!!! danger "Out-of-bounds access is undefined behavior — do not run this"
    The following fragment illustrates the mistake; it is shown for explanation only, and deliberately should not be relied upon to behave any particular way if actually executed:

    ```c
    int scores[5] = {91, 85, 77, 100, 63};
    scores[5] = 50;       // BUG: index 5 is out of bounds (valid indices are 0-4)
    printf("%d\n", scores[10]); // BUG: reads far outside the array entirely
    ```

    Neither line will necessarily produce an error message or a crash — that is precisely what makes out-of-bounds access so dangerous. It may silently overwrite some other variable's memory, producing a bug that only manifests much later and in a seemingly unrelated part of the program. Always ensure index expressions stay within `0` to `size - 1`, and be especially careful with loop conditions (`<` versus `<=`) that control array indices.

## Multidimensional Arrays

A **2D array** models a grid — rows and columns — and is declared with two size specifiers:

```c
#include <stdio.h>

int main(void) {
    int grid[3][4] = {
        {1, 2, 3, 4},
        {5, 6, 7, 8},
        {9, 10, 11, 12}
    };

    for (int row = 0; row < 3; row++) {
        for (int col = 0; col < 4; col++) {
            printf("%4d", grid[row][col]);
        }
        printf("\n");
    }

    return 0;
}
```

### Compile and run

```bash
gcc -std=c11 -Wall -Wextra array_2d.c -o array_2d
./array_2d
```

### Expected output

```text
   1   2   3   4
   5   6   7   8
   9  10  11  12
```

`grid[3][4]` declares 3 rows of 4 columns each — 12 `int`s in total. Despite the two-dimensional notation, the elements are actually stored in one single, contiguous block of memory, arranged in **row-major order**: the entire first row's 4 elements come first, immediately followed by the entire second row's 4 elements, then the third row's. `grid[1][2]` (value `7`) is therefore the 7th element overall in that underlying contiguous block (counting from 1): `(1 * 4) + 2 + 1`. Understanding this layout becomes important once we discuss pointers in Chapter 11.

## Arrays and Functions: The Decay Pattern

When you pass an array to a function, C does **not** copy the entire array (unlike passing an ordinary variable, which Chapter 8 showed is always copied). Instead, the array **decays** to a pointer to its first element — the function receives just enough information to access the array's elements, but it has no way of knowing, from the array alone, how many elements there actually are.

```c
#include <stdio.h>

void print_array(int arr[], int size) {
    for (int i = 0; i < size; i++) {
        printf("%d ", arr[i]);
    }
    printf("\n");
}

int main(void) {
    int numbers[6] = {3, 1, 4, 1, 5, 9};

    print_array(numbers, 6); // must pass the size separately

    return 0;
}
```

### Compile and run

```bash
gcc -std=c11 -Wall -Wextra array_function.c -o array_function
./array_function
```

### Expected output

```text
3 1 4 1 5 9
```

Notice the function signature `void print_array(int arr[], int size)` takes the size as a separate, explicit parameter — this is not optional convenience, it is **required**, because `arr` inside the function carries no size information whatsoever. The full mechanics of exactly what "decays to a pointer" means, and how pointers and arrays relate at the level of memory addresses, is the subject of Chapter 11; for now, the practical rule to remember is simply: **always pass an array's size alongside the array itself.**

### `sizeof` on a Full Array vs. an Array Parameter

This decay behavior has a very concrete, observable consequence with `sizeof` (introduced in Chapter 3):

```c
#include <stdio.h>

void check_size(int arr[]) {
    printf("Inside check_size, sizeof(arr) = %zu\n", sizeof(arr));
}

int main(void) {
    int numbers[6] = {3, 1, 4, 1, 5, 9};

    printf("Inside main, sizeof(numbers) = %zu\n", sizeof(numbers));
    check_size(numbers);

    return 0;
}
```

### Compile and run

```bash
gcc -std=c11 -Wall -Wextra sizeof_decay.c -o sizeof_decay
./sizeof_decay
```

### Expected output (typical on a 64-bit system)

```text
Inside main, sizeof(numbers) = 24
Inside check_size, sizeof(arr) = 8
```

Inside `main`, `numbers` is still a genuine array of 6 `int`s, so `sizeof(numbers)` correctly reports its total size in bytes (`6 * 4 = 24` bytes, assuming a 4-byte `int`). But inside `check_size`, the parameter `arr` is not actually an array at all — due to decay, it is really a pointer, so `sizeof(arr)` reports the size of a pointer on this system (`8` bytes on a typical 64-bit machine), with no relationship to how many elements the original array had. This is precisely why the size must always be passed as a separate parameter: `sizeof` cannot rescue you once you're inside the function.

!!! warning "Exact `sizeof` numbers are platform-dependent"
    The specific numbers `24` and `8` above depend on the platform: `int` is 4 bytes on essentially all common modern systems, but a pointer is 8 bytes on a 64-bit system and would be 4 bytes on a 32-bit system. The important, portable lesson is the *relationship* — `sizeof` on the real array reflects its true size, while `sizeof` on the decayed parameter reflects only a pointer's size — not the specific byte counts themselves.

## Worked Algorithms

### Linear Search

```c
#include <stdio.h>

int linear_search(int arr[], int size, int target) {
    for (int i = 0; i < size; i++) {
        if (arr[i] == target) {
            return i; // found it; return its index
        }
    }
    return -1; // not found
}

int main(void) {
    int numbers[7] = {12, 45, 7, 23, 56, 9, 34};

    int index = linear_search(numbers, 7, 23);

    if (index != -1) {
        printf("Found 23 at index %d\n", index);
    } else {
        printf("23 not found\n");
    }

    return 0;
}
```

### Compile and run

```bash
gcc -std=c11 -Wall -Wextra linear_search.c -o linear_search
./linear_search
```

### Expected output

```text
Found 23 at index 3
```

### Finding Max and Min

```c
#include <stdio.h>

int main(void) {
    int values[6] = {42, 17, 89, 3, 56, 24};

    int max = values[0];
    int min = values[0];

    for (int i = 1; i < 6; i++) {
        if (values[i] > max) {
            max = values[i];
        }
        if (values[i] < min) {
            min = values[i];
        }
    }

    printf("Max: %d, Min: %d\n", max, min);

    return 0;
}
```

### Compile and run

```bash
gcc -std=c11 -Wall -Wextra min_max.c -o min_max
./min_max
```

### Expected output

```text
Max: 89, Min: 3
```

Both `max` and `min` are initialized to `values[0]` (the first element), and the loop starts from index `1`, since there is no need to compare the first element against itself.

### Reversing an Array in Place

```c
#include <stdio.h>

void reverse_array(int arr[], int size) {
    int start = 0;
    int end = size - 1;

    while (start < end) {
        int temp = arr[start];
        arr[start] = arr[end];
        arr[end] = temp;

        start++;
        end--;
    }
}

int main(void) {
    int numbers[5] = {1, 2, 3, 4, 5};

    reverse_array(numbers, 5);

    for (int i = 0; i < 5; i++) {
        printf("%d ", numbers[i]);
    }
    printf("\n");

    return 0;
}
```

### Compile and run

```bash
gcc -std=c11 -Wall -Wextra reverse_array.c -o reverse_array
./reverse_array
```

### Expected output

```text
5 4 3 2 1
```

"In place" means the original array itself is modified directly — no second array is created. This works here (unlike the failed-swap example from Chapter 8) because `arr` decaying to a pointer means the function is working with the *same underlying memory* as `main`'s `numbers` array, not a copy of it; modifying `arr[i]` inside the function genuinely modifies `numbers[i]` back in `main`. This is an important exception to the pass-by-value rule, and it is explained fully once we cover pointers in Chapter 11.

### Bubble Sort

```c
#include <stdio.h>

void bubble_sort(int arr[], int size) {
    for (int pass = 0; pass < size - 1; pass++) {
        for (int i = 0; i < size - 1 - pass; i++) {
            if (arr[i] > arr[i + 1]) {
                int temp = arr[i];
                arr[i] = arr[i + 1];
                arr[i + 1] = temp;
            }
        }
    }
}

int main(void) {
    int numbers[6] = {64, 25, 12, 22, 11, 90};

    bubble_sort(numbers, 6);

    printf("Sorted array: ");
    for (int i = 0; i < 6; i++) {
        printf("%d ", numbers[i]);
    }
    printf("\n");

    return 0;
}
```

### Compile and run

```bash
gcc -std=c11 -Wall -Wextra bubble_sort.c -o bubble_sort
./bubble_sort
```

### Expected output

```text
Sorted array: 11 12 22 25 64 90
```

Bubble sort repeatedly steps through the array, comparing each adjacent pair of elements and swapping them if they're out of order; each full pass "bubbles" the largest remaining unsorted value to its correct position at the end, which is why the inner loop's range shrinks by one (`size - 1 - pass`) on each subsequent pass — the last `pass` elements are already known to be correctly sorted and don't need rechecking.

## Summary

- An array is a fixed-size, contiguous block of memory holding multiple values of the same type, indexed from `0` to `size - 1`.
- Initializer lists can be full, partial (remaining elements default to `0`), or `{0}` to zero an entire array; omitting an explicit size with a full initializer list lets the compiler count elements for you.
- C performs no automatic bounds checking; reading or writing outside `0` to `size - 1` is undefined behavior and can silently corrupt memory rather than producing an obvious error.
- 2D arrays model grids and are stored as one contiguous block in row-major order — the entire first row, then the entire second row, and so on.
- Passing an array to a function decays it to a pointer to its first element, which carries no size information, so the size must always be passed as a separate parameter; `sizeof` on that parameter reports a pointer's size, not the array's, reinforcing that decay has occurred. Full pointer mechanics are covered in Chapter 11.
- Linear search, max/min, in-place reversal, and bubble sort are foundational array algorithms worth knowing by hand before relying on library or standard-data-structure equivalents later.

## Exercises

1. Declare an array of 10 integers, fill it using a loop with the squares of `0` through `9` (i.e., `0, 1, 4, 9, ...`), and print it.
2. Write a function `int sum_array(int arr[], int size)` that returns the sum of all elements, and test it on at least two different arrays of different sizes.
3. Write a program that declares a `4x4` 2D array of integers, fills it so that `grid[row][col] = row * 4 + col`, and prints it formatted as a grid (recall `%4d` from Chapter 7).
4. Modify the linear search worked example to count and print how many comparisons (`arr[i] == target` checks) were performed before the target was found (or before concluding it wasn't present).
5. *(Harder)* Write a function that checks whether an array is already sorted in ascending order, without modifying it, and use it to print "already sorted" or "not sorted" before and after calling `bubble_sort` on a test array.
6. *(Harder)* Write a program that reads up to 20 integers from the user into an array (stopping early if the user enters `-1`, tracking how many were actually entered in a separate `count` variable), then prints the array's maximum, minimum, and sum. Be careful that every function you write to process this array takes both the array and the actual `count`, not the array's full declared capacity of 20.

---
[← Chapter 8 — Functions and Modular Programming](chapter-08.md) | [Chapter 10 — Strings →](chapter-10.md)
