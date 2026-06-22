# Chapter 7 — Loops and Iteration

## Introduction

Decisions, covered in Chapter 6, let a program choose between different paths. Loops let a program **repeat** a path multiple times without writing the same code over and over. This chapter covers C's three loop constructs — `while`, `do-while`, and `for` — along with `break`, `continue`, nested loops, and the patterns (and pitfalls) of writing loops that terminate exactly when intended.

## The `while` Loop

A `while` loop repeats a block of statements for as long as a condition remains true. The condition is checked **before** each iteration (including the very first), so if the condition is false from the start, the loop body never runs at all.

```c
#include <stdio.h>

int main(void) {
    int count = 1;

    while (count <= 5) {
        printf("Count is %d\n", count);
        count = count + 1;
    }

    printf("Loop finished.\n");

    return 0;
}
```

### Compile and run

```bash
gcc -std=c11 -Wall -Wextra while_basic.c -o while_basic
./while_basic
```

### Expected output

```text
Count is 1
Count is 2
Count is 3
Count is 4
Count is 5
Loop finished.
```

Three things must be present for a `while` loop to behave correctly: a starting value for the variable controlling the loop (`count = 1`), a condition that eventually becomes false (`count <= 5`), and something inside the loop body that moves the variable toward that false condition (`count = count + 1`). Forgetting the last of these is one of the most common ways to accidentally write an infinite loop, covered later in this chapter.

## The `do-while` Loop

A `do-while` loop is almost identical to `while`, except the condition is checked **after** each iteration, so the loop body is guaranteed to run at least once, even if the condition is false from the start.

```c
#include <stdio.h>

int main(void) {
    int count = 10;

    do {
        printf("Count is %d\n", count);
        count = count + 1;
    } while (count <= 5);

    printf("Loop finished.\n");

    return 0;
}
```

### Compile and run

```bash
gcc -std=c11 -Wall -Wextra do_while_basic.c -o do_while_basic
./do_while_basic
```

### Expected output

```text
Count is 10
Loop finished.
```

Even though `count <= 5` is false from the very beginning (`count` starts at `10`), the body still ran exactly once before the condition was ever checked. Note the required semicolon after `while (condition)` in a `do-while` — easy to forget, since ordinary `while` loops and `for` loops don't take one there.

### When "Run at Least Once" Matters: Menu Loops

The "always runs at least once" property is exactly what you want for an interactive menu: you must show the menu and read the user's choice at least once before you have any value to test in a loop condition.

```c
#include <stdio.h>

int main(void) {
    int choice;

    do {
        printf("\n--- Menu ---\n");
        printf("1. Say hello\n");
        printf("2. Say goodbye\n");
        printf("0. Exit\n");
        printf("Enter choice: ");
        scanf("%d", &choice);

        if (choice == 1) {
            printf("Hello!\n");
        } else if (choice == 2) {
            printf("Goodbye!\n");
        } else if (choice != 0) {
            printf("Unknown choice.\n");
        }
    } while (choice != 0);

    printf("Exiting program.\n");

    return 0;
}
```

### Compile and run

```bash
gcc -std=c11 -Wall -Wextra menu_loop.c -o menu_loop
./menu_loop
```

### Example interactive session

```text
--- Menu ---
1. Say hello
2. Say goodbye
0. Exit
Enter choice: 1
Hello!

--- Menu ---
1. Say hello
2. Say goodbye
0. Exit
Enter choice: 0
Exiting program.
```

With a plain `while` loop, you would need to somehow read a choice *before* the loop just to have something to test in the condition, then read it again inside the loop — `do-while` avoids that awkward duplication entirely.

## The `for` Loop

A `for` loop packages the three parts every counting loop needs — initialization, condition, and update — into one compact header:

```c
for (initialization; condition; update) {
    // body
}
```

- **initialization** runs exactly once, before the loop starts (typically declaring and setting a counter variable).
- **condition** is checked before every iteration, exactly like `while` — if false, the loop ends (or never runs at all).
- **update** runs after every iteration's body completes, before the condition is checked again (typically incrementing or decrementing the counter).

```c
#include <stdio.h>

int main(void) {
    for (int i = 1; i <= 5; i = i + 1) {
        printf("i is %d\n", i);
    }

    printf("Loop finished.\n");

    return 0;
}
```

### Compile and run

```bash
gcc -std=c11 -Wall -Wextra for_basic.c -o for_basic
./for_basic
```

### Expected output

```text
i is 1
i is 2
i is 3
i is 4
i is 5
Loop finished.
```

This is exactly equivalent to the `while` example earlier, just with all three moving parts gathered in one place at the top, which is why `for` is the conventional choice whenever the number of iterations is known or counted in advance. `i = i + 1` is very commonly written using the increment operator `i++` instead (introduced in Chapter 4) — both have the same effect here.

!!! note "Scope of the loop variable"
    A variable declared inside the `for` header, like `int i` above, exists only for the duration of the loop (including its body) — it cannot be accessed before or after the loop. This is generally good practice, since it prevents the counter from being accidentally reused or misread elsewhere in the function. Full detail on variable scope follows in Chapter 8 and again in Chapter 16.

## `break` and `continue`

`break` immediately exits the nearest enclosing loop entirely, skipping any remaining iterations. `continue` skips the *rest of the current iteration only* and jumps straight to the next one (re-checking the condition, and for a `for` loop, running the update step first).

```c
#include <stdio.h>

int main(void) {
    printf("Using break:\n");
    for (int i = 1; i <= 10; i++) {
        if (i == 5) {
            break;
        }
        printf("%d\n", i);
    }

    printf("\nUsing continue:\n");
    for (int i = 1; i <= 10; i++) {
        if (i == 5) {
            continue;
        }
        printf("%d\n", i);
    }

    return 0;
}
```

### Compile and run

```bash
gcc -std=c11 -Wall -Wextra break_continue.c -o break_continue
./break_continue
```

### Expected output

```text
Using break:
1
2
3
4

Using continue:
1
2
3
4
6
7
8
9
10
```

The `break` loop stops completely the moment `i` reaches `5` — nothing from `5` onward ever prints, including `6` through `10`. The `continue` loop, by contrast, only skips printing `5` itself; the loop keeps going afterward and prints `6` through `10` normally. This distinction — "stop everything" versus "skip just this once" — is the entire difference between the two.

## Nested Loops

A loop body can contain another complete loop. The inner loop runs to completion once for every single iteration of the outer loop.

### Worked Example: Multiplication Table

```c
#include <stdio.h>

int main(void) {
    for (int row = 1; row <= 5; row++) {
        for (int col = 1; col <= 5; col++) {
            printf("%4d", row * col);
        }
        printf("\n");
    }

    return 0;
}
```

### Compile and run

```bash
gcc -std=c11 -Wall -Wextra multiplication_table.c -o multiplication_table
./multiplication_table
```

### Expected output

```text
   1   2   3   4   5
   2   4   6   8  10
   3   6   9  12  15
   4   8  12  16  20
   5  10  15  20  25
```

For each value of `row` (the outer loop), the entire inner loop runs through every value of `col` from `1` to `5` before the outer loop advances to the next `row`. The `%4d` format specifier (Chapter 5) right-aligns each number in a 4-character-wide field, keeping the columns neatly lined up.

### Worked Example: Triangle/Pyramid Pattern

```c
#include <stdio.h>

int main(void) {
    int rows = 5;

    for (int i = 1; i <= rows; i++) {
        for (int j = 1; j <= i; j++) {
            printf("*");
        }
        printf("\n");
    }

    return 0;
}
```

### Compile and run

```bash
gcc -std=c11 -Wall -Wextra triangle_pattern.c -o triangle_pattern
./triangle_pattern
```

### Expected output

```text
*
**
***
****
*****
```

The outer loop variable `i` controls which row we're on; the inner loop prints exactly `i` stars before moving to the next row — a direct, concrete example of an inner loop's behavior depending on the outer loop's current state.

## Infinite Loops

An infinite loop is one whose condition never becomes false, so it never ends on its own. This happens **accidentally** most often when the update step is missing or wrong:

```c
#include <stdio.h>

int main(void) {
    int count = 1;

    while (count <= 5) {
        printf("Count is %d\n", count);
        // BUG: forgot to update count, so the condition is always true
    }

    return 0;
}
```

!!! danger "Accidental infinite loop"
    The program above would print `Count is 1` forever and never reach `printf("Loop finished.\n")` or `return 0;` — it would have to be forcibly terminated (for example, with Ctrl+C in the terminal). This example is deliberately **not meant to be run as shown** for that reason; it's included purely to illustrate the bug. The fix is simply to remember the update step, `count = count + 1;`, inside the loop body.

### The Legitimate Pattern: `for (;;)` / `while (1)` with an Explicit `break`

Sometimes you genuinely don't know in advance how many iterations are needed — for example, "keep asking the user for input until they type a value that makes sense." The idiomatic way to express "loop forever until some condition detected inside the loop body says to stop" is an intentionally infinite loop header paired with an explicit `break`:

```c
#include <stdio.h>

int main(void) {
    int number;

    for (;;) {
        printf("Enter a positive number (or 0 to stop): ");
        scanf("%d", &number);

        if (number == 0) {
            break;
        }

        printf("You entered: %d\n", number);
    }

    printf("Done.\n");

    return 0;
}
```

### Compile and run

```bash
gcc -std=c11 -Wall -Wextra infinite_with_break.c -o infinite_with_break
./infinite_with_break
```

### Example interactive session

```text
Enter a positive number (or 0 to stop): 7
You entered: 7
Enter a positive number (or 0 to stop): 12
You entered: 12
Enter a positive number (or 0 to stop): 0
Done.
```

`for (;;)` has all three header parts left empty, which C treats as "no condition to check," meaning it is always true — equally common is `while (1)`, since `1` is always true (recall truthiness from Chapter 6). Neither form is a bug **as long as** the loop body contains a `break` (or a `return`) that is actually reachable and will eventually execute. The difference between this and the accidental infinite loop above is entirely about whether termination is deliberately designed in.

## Worked Examples

### Sum of Digits of a Number

```c
#include <stdio.h>

int main(void) {
    int number;

    printf("Enter a positive integer: ");
    scanf("%d", &number);

    int sum = 0;
    int n = number;

    while (n > 0) {
        int digit = n % 10;
        sum = sum + digit;
        n = n / 10;
    }

    printf("Sum of digits of %d is %d\n", number, sum);

    return 0;
}
```

### Compile and run

```bash
gcc -std=c11 -Wall -Wextra sum_of_digits.c -o sum_of_digits
./sum_of_digits
```

### Example interactive session

```text
Enter a positive integer: 4827
Sum of digits of 4827 is 21
```

Each iteration peels off the last digit with `n % 10` (modulo, from Chapter 4), adds it to the running total, then removes that digit from `n` with integer division `n / 10`. The loop ends once `n` has been reduced all the way to `0`.

### Checking if a Number Is Prime

```c
#include <stdio.h>

int main(void) {
    int number;

    printf("Enter a positive integer greater than 1: ");
    scanf("%d", &number);

    int is_prime = 1; // assume true until proven otherwise

    for (int divisor = 2; divisor < number; divisor++) {
        if (number % divisor == 0) {
            is_prime = 0;
            break; // no need to keep checking once we've found a divisor
        }
    }

    if (is_prime) {
        printf("%d is prime.\n", number);
    } else {
        printf("%d is not prime.\n", number);
    }

    return 0;
}
```

### Compile and run

```bash
gcc -std=c11 -Wall -Wextra prime_check.c -o prime_check
./prime_check
```

### Example interactive session

```text
Enter a positive integer greater than 1: 17
17 is prime.
```

`is_prime` starts as `1` (true) and is only ever set to `0` if some `divisor` between `2` and `number - 1` divides `number` evenly. The `break` immediately after finding a divisor is a worthwhile efficiency: once we know the number isn't prime, there's no reason to keep checking further divisors.

## Which Loop Should I Use? A Decision Guide

- Use a **`for` loop** when you know (or can compute) the number of iterations in advance — counting up or down, iterating over a fixed range, walking through an array (Chapter 9). This is by far the most common case.
- Use a **`while` loop** when the loop should continue based on a condition that isn't a simple counter, and the body may legitimately need to run zero times — for example, processing input "while there is more input to read."
- Use a **`do-while` loop** specifically when the body must run at least once before the condition can even be meaningfully checked — the canonical example being a menu that must be displayed before the user can choose to exit it.
- Reach for `for (;;)` or `while (1)` with an internal `break` when the natural stopping condition is discovered partway through the loop body itself, rather than being known at the top.

## Summary

- `while` checks its condition before each iteration (may run zero times); `do-while` checks after each iteration (always runs at least once) — useful for menus and similar "must happen once" loops.
- `for (init; condition; update)` packages a counter's setup, test, and step into one header; it is the standard choice for counted iteration, and its loop variable's scope is limited to the loop.
- `break` exits the nearest loop entirely; `continue` skips only the remainder of the current iteration and moves on to the next one — these are distinct and not interchangeable.
- Nested loops run the inner loop to completion for every single iteration of the outer loop; this is the basis of multiplication tables, patterns, and (later) processing 2D arrays.
- Infinite loops happen accidentally when a loop's condition never becomes false (commonly: a forgotten update step); the same "always true" condition (`for (;;)`, `while (1)`) is also a legitimate, idiomatic pattern when paired with a deliberate, reachable `break`.
- Choose `for` for counted iteration, `while` for condition-driven iteration that might run zero times, `do-while` for "must run at least once," and an infinite-loop-plus-`break` when the stopping point is discovered inside the body.

## Exercises

1. Write a `for` loop that prints all even numbers from 2 to 20 inclusive.
2. Write a `do-while` loop-based program that repeatedly asks the user to enter a number, and stops as soon as the user enters a negative number, then prints the count of positive numbers they entered.
3. Modify the multiplication table worked example to take the table size as input from the user instead of a fixed `5`.
4. Write a program using nested loops that prints a right-aligned pyramid of stars, like this, for a height of 5 read from the user:
   ```text
       *
      ***
     *****
    *******
   *********
   ```
   (Hint: each row needs both leading spaces and an odd number of stars; work out the pattern for a few rows on paper first.)
5. *(Harder)* Write a program that uses `continue` inside a `for` loop from 1 to 50 to print only the numbers that are **not** divisible by 3 or 5.
6. *(Harder)* Write a program using a `for (;;)` loop with an internal `break` that repeatedly reads two integers and prints their greatest common divisor (you may use repeated subtraction or the modulo operator to compute it), stopping when the user enters `0` for the first integer.

---
[← Chapter 6 — Decision Making (if, else, switch)](chapter-06.md) | [Chapter 8 — Functions and Modular Programming →](chapter-08.md)
