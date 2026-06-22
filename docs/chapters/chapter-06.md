# Chapter 6 — Decision Making (if, else, switch)

## Introduction

Every program we have written so far executes the same statements, in the same order, every single time it runs. Real programs need to make decisions: charge a different shipping rate depending on the order total, print a different message depending on a grade, or take a different branch of logic depending on user input. This chapter covers C's primary decision-making tools: the `if` statement, `if`-`else` chains, nested `if`s, and the `switch` statement, along with the most common bugs beginners write when first using them.

## Truthiness in C

Before C had a real boolean type, decisions in C were made entirely using the rule: **the value `0` means false; any non-zero value means true.** This rule still underlies everything in modern C, even now that a boolean type exists.

```c
#include <stdio.h>

int main(void) {
    int zero = 0;
    int five = 5;
    int negative_one = -1;

    if (zero) {
        printf("zero is true\n");
    } else {
        printf("zero is false\n");
    }

    if (five) {
        printf("five is true\n");
    } else {
        printf("five is false\n");
    }

    if (negative_one) {
        printf("negative_one is true\n");
    } else {
        printf("negative_one is false\n");
    }

    return 0;
}
```

### Compile and run

```bash
gcc -std=c11 -Wall -Wextra truthiness.c -o truthiness
./truthiness
```

### Expected output

```text
zero is false
five is true
negative_one is true
```

Notice that `-1` counts as true. Only exactly `0` is false; every other value, positive or negative, counts as true. This applies to the results of comparisons too: `5 > 3` evaluates to the `int` value `1`, and `5 < 3` evaluates to the `int` value `0`.

!!! note "`<stdbool.h>`"
    C99 introduced `<stdbool.h>`, which gives you the names `bool`, `true`, and `false` as convenient aliases — `true` is defined as `1`, `false` as `0`, and `bool` is really just a restricted integer type under the hood (recall this from Chapter 3). Using `bool`, `true`, and `false` does not change how truthiness works; it only makes code that represents a yes/no condition easier to read. We'll use `bool` freely from here on whenever a variable's purpose is to represent a true/false condition.

## The `if` Statement

The simplest decision construct is `if`: a block of code runs only if a condition (any expression — typically a comparison) is true.

```c
#include <stdio.h>

int main(void) {
    int temperature = 38;

    if (temperature > 35) {
        printf("It's a hot day.\n");
    }

    printf("Program finished.\n");

    return 0;
}
```

### Compile and run

```bash
gcc -std=c11 -Wall -Wextra if_basic.c -o if_basic
./if_basic
```

### Expected output

```text
It's a hot day.
Program finished.
```

If `temperature` were `20` instead, the `printf` inside the `if` block would simply be skipped, and only `Program finished.` would print.

!!! tip "Always use braces, even for one-line bodies"
    C allows you to omit the braces `{ }` around an `if` body that contains only a single statement. This is legal but risky: if you later add a second statement without adding braces, only the first statement is actually part of the `if`. Writing `{ }` consistently, even for one-line bodies, avoids this entire class of bug and is standard practice in production code.

## `if`-`else`

`if`-`else` lets you specify an alternative block of code to run when the condition is false.

```c
#include <stdio.h>

int main(void) {
    int age = 16;

    if (age >= 18) {
        printf("You are an adult.\n");
    } else {
        printf("You are a minor.\n");
    }

    return 0;
}
```

### Compile and run

```bash
gcc -std=c11 -Wall -Wextra if_else.c -o if_else
./if_else
```

### Expected output

```text
You are a minor.
```

## `else if` Ladders

When there are more than two possible outcomes, you chain `else if` clauses together. Each condition is tested in order, top to bottom, and the first one that is true has its block executed; all the rest are skipped. An optional final `else` catches everything not matched by any earlier branch.

### Worked Example 1: Grade Calculator

```c
#include <stdio.h>

int main(void) {
    int score;

    printf("Enter your exam score (0-100): ");
    scanf("%d", &score);

    if (score >= 90) {
        printf("Grade: A\n");
    } else if (score >= 80) {
        printf("Grade: B\n");
    } else if (score >= 70) {
        printf("Grade: C\n");
    } else if (score >= 60) {
        printf("Grade: D\n");
    } else {
        printf("Grade: F\n");
    }

    return 0;
}
```

### Compile and run

```bash
gcc -std=c11 -Wall -Wextra grade_calculator.c -o grade_calculator
./grade_calculator
```

### Example interactive session

```text
Enter your exam score (0-100): 84
Grade: B
```

Trace through this carefully: with `score = 84`, the first condition `score >= 90` is false, so control moves to `score >= 80`, which is true, so `Grade: B` prints and **every remaining `else if`/`else` is skipped automatically** — `score >= 70` is never even checked, even though `84 >= 70` is also true. Order matters in an `else if` ladder: conditions are tested from top to bottom, and only the first match wins.

!!! warning "Order matters"
    If you had instead written the conditions from loosest to tightest — checking `score >= 60` before `score >= 90` — every score of 60 or above would incorrectly report `Grade: D`, because that branch would match first and all the later, more specific branches would never be reached. When writing an `else if` ladder over overlapping ranges, order conditions from most specific to least specific (or, as here, simply from highest threshold to lowest).

## Nested `if` Statements

An `if` or `else` block can itself contain another complete `if` statement. This is useful when a decision genuinely depends on more than one condition in a hierarchical way.

```c
#include <stdio.h>

int main(void) {
    int age = 20;
    int has_ticket = 1;

    if (age >= 18) {
        if (has_ticket) {
            printf("Welcome, please enter.\n");
        } else {
            printf("You need a ticket to enter.\n");
        }
    } else {
        printf("You must be 18 or older to enter.\n");
    }

    return 0;
}
```

### Compile and run

```bash
gcc -std=c11 -Wall -Wextra nested_if.c -o nested_if
./nested_if
```

### Expected output

```text
Welcome, please enter.
```

The outer `if` checks age first; only if that passes does the program even look at whether there is a ticket. This is different from a flat `else if` ladder, because the second condition is only meaningful in the context of the first one already having been true.

!!! warning "The dangling-else ambiguity"
    When `if` statements are nested without braces, it can become genuinely ambiguous (to a human reader, though not to the compiler, which has a fixed rule) which `if` an `else` belongs to:

    ```c
    if (age >= 18)
        if (has_ticket)
            printf("Welcome, please enter.\n");
    else
        printf("You must be 18 or older to enter.\n");
    ```

    Despite the indentation suggesting the `else` belongs to the outer `if (age >= 18)`, C's actual rule is that an `else` always binds to the **nearest preceding `if` that doesn't already have an `else`** — so this `else` actually belongs to `if (has_ticket)`, not `if (age >= 18)`. This means an adult without a ticket would see nothing printed at all, while a minor would also see nothing printed (since the outer `if` fails entirely) — definitely not the intended behavior. Indentation is just whitespace to the compiler; it does not determine which `if` an `else` attaches to. **Always use braces `{ }` around nested `if` bodies** to make the structure unambiguous both to the compiler's actual rule and to anyone reading the code.

## The `=` vs `==` Bug, Live

This is one of the most notorious bugs in C, introduced briefly in Chapter 4. Here it is shown actually happening:

```c
#include <stdio.h>

int main(void) {
    int password_attempt = 1234;
    int correct_password = 4321;

    if (password_attempt = correct_password) {
        printf("Access granted!\n");
    } else {
        printf("Access denied.\n");
    }

    printf("password_attempt is now: %d\n", password_attempt);

    return 0;
}
```

### Compile and run

```bash
gcc -std=c11 -Wall -Wextra equals_bug.c -o equals_bug
./equals_bug
```

### Expected output

```text
Access granted!
password_attempt is now: 4321
```

This is badly wrong: `password_attempt` (`1234`) does not equal `correct_password` (`4321`), yet `Access granted!` printed anyway, and the final line proves why — `password_attempt` was overwritten with `4321` by the `=` inside the condition. The expression `password_attempt = correct_password` is an assignment, not a comparison: it stores `4321` into `password_attempt` and then evaluates to the value just stored, `4321`, which is non-zero, hence "true." The fix is to use `==`:

```c
#include <stdio.h>

int main(void) {
    int password_attempt = 1234;
    int correct_password = 4321;

    if (password_attempt == correct_password) {
        printf("Access granted!\n");
    } else {
        printf("Access denied.\n");
    }

    return 0;
}
```

### Compile and run

```bash
gcc -std=c11 -Wall -Wextra equals_fixed.c -o equals_fixed
./equals_fixed
```

### Expected output

```text
Access denied.
```

!!! tip "Let the compiler help"
    Most compilers, including gcc with `-Wall`, will warn about a plain assignment used as an `if` condition (something like `warning: suggest parentheses around assignment used as truth value`). Never ignore this warning.

## The `switch` Statement

`switch` is an alternative to long `if`-`else` ladders, specifically for comparing one value against several possible exact matches. Its syntax is:

```c
switch (expression) {
    case value1:
        // statements
        break;
    case value2:
        // statements
        break;
    default:
        // statements
}
```

`expression` is evaluated once, then compared against each `case` value (which must be a constant — a literal or a named constant, not a variable) in turn. When a match is found, execution jumps to that `case` and continues from there. `default` is optional and catches any value not matched by any `case`.

```c
#include <stdio.h>

int main(void) {
    int day = 3;

    switch (day) {
        case 1:
            printf("Monday\n");
            break;
        case 2:
            printf("Tuesday\n");
            break;
        case 3:
            printf("Wednesday\n");
            break;
        case 4:
            printf("Thursday\n");
            break;
        case 5:
            printf("Friday\n");
            break;
        default:
            printf("Weekend\n");
    }

    return 0;
}
```

### Compile and run

```bash
gcc -std=c11 -Wall -Wextra switch_basic.c -o switch_basic
./switch_basic
```

### Expected output

```text
Wednesday
```

### Fall-Through: What Happens Without `break`

Each `case` label is just a jump target — execution does **not** automatically stop at the end of a `case`. Without an explicit `break`, control falls through into the next `case`'s statements, running them too, regardless of whether their label matches. This is called **fall-through**, and it is a frequent source of bugs for beginners who forget `break`.

```c
#include <stdio.h>

int main(void) {
    int day = 3;

    switch (day) {
        case 1:
            printf("Monday\n");
        case 2:
            printf("Tuesday\n");
        case 3:
            printf("Wednesday\n");
        case 4:
            printf("Thursday\n");
        case 5:
            printf("Friday\n");
        default:
            printf("Weekend\n");
    }

    return 0;
}
```

### Compile and run

```bash
gcc -std=c11 -Wall -Wextra switch_fallthrough.c -o switch_fallthrough
./switch_fallthrough
```

### Expected output

```text
Wednesday
Thursday
Friday
Weekend
```

With `day = 3`, execution jumps to `case 3:` and prints `Wednesday`, but with no `break` to stop it, it then keeps running every statement below — `case 4`, `case 5`, and `default` — even though `day` does not equal `4` or `5`. This is almost never what a beginner intends.

!!! danger "Missing `break` is a classic, hard-to-spot bug"
    A `switch` with a missing `break` will still compile cleanly and often appears to work during quick testing if you happen to test only the last `case`. The bug surfaces only when a different `case` is hit, making it the kind of mistake that slips past casual testing. Get in the habit of writing `break;` as soon as you write a `case` label, before filling in the body, so you never forget it. (gcc's `-Wextra` will warn about implicit fall-through in many cases with `-Wimplicit-fallthrough`, but do not rely on the warning instead of writing the `break`.)

!!! note "Intentional fall-through"
    Fall-through is occasionally used deliberately, for example to let several `case` labels share one block of code:

    ```c
    switch (day) {
        case 6:
        case 7:
            printf("Weekend\n");
            break;
        default:
            printf("Weekday\n");
    }
    ```

    Here, `case 6:` has no body of its own, so it falls through into `case 7:`'s body intentionally — both `6` and `7` print `Weekend`. This stacking of empty `case` labels is the one fall-through pattern considered good style, precisely because it is visually obvious and clearly intentional.

### When `switch` Is Preferable to `if`-`else`

`switch` is a good fit when you are comparing **one single value** against several **specific constant values** — like a menu choice, a day number, or a single character command. It tends to read more clearly than an equivalent `else if` ladder in that situation, and some compilers can optimize a `switch` over many cases more efficiently than a long chain of comparisons. `switch` is a poor fit (or simply impossible) when conditions involve ranges (`score >= 80`), multiple variables, or any non-constant comparison — for those, an `if`-`else` ladder is the right tool.

## Worked Example 2: Text-Menu Calculator

```c
#include <stdio.h>

int main(void) {
    char operation;
    double a, b;

    printf("Enter first number: ");
    scanf("%lf", &a);

    printf("Enter an operation (+, -, *, /): ");
    scanf(" %c", &operation);

    printf("Enter second number: ");
    scanf("%lf", &b);

    switch (operation) {
        case '+':
            printf("Result: %.2f\n", a + b);
            break;
        case '-':
            printf("Result: %.2f\n", a - b);
            break;
        case '*':
            printf("Result: %.2f\n", a * b);
            break;
        case '/':
            if (b == 0) {
                printf("Error: division by zero.\n");
            } else {
                printf("Result: %.2f\n", a / b);
            }
            break;
        default:
            printf("Unknown operation '%c'.\n", operation);
    }

    return 0;
}
```

### Compile and run

```bash
gcc -std=c11 -Wall -Wextra menu_calculator.c -o menu_calculator
./menu_calculator
```

### Example interactive session

```text
Enter first number: 12
Enter an operation (+, -, *, /): *
Enter second number: 4
Result: 48.00
```

Notice the `scanf(" %c", &operation)` uses the leading-space trick from Chapter 5 to skip the leftover newline from the previous numeric read, and the `/` case nests a plain `if`-`else` inside the `switch` to guard against dividing by zero — a good illustration of `switch` and `if` working together, each used where it fits best.

## Summary

- C has no separate "true"/"false" values prior to `<stdbool.h>`: `0` is false, and any non-zero value is true; `<stdbool.h>` (C99) supplies `bool`, `true`, and `false` as readable aliases over this same rule.
- `if` runs a block only when its condition is true; `if`-`else` adds an alternative; `else if` chains test conditions top to bottom, and only the first match runs — order conditions carefully when ranges overlap.
- Nested `if` statements let a decision depend on more than one condition hierarchically; always brace nested bodies to avoid the dangling-else ambiguity, where an `else` silently binds to the nearest unmatched `if`, not the one indentation suggests.
- `x = 5` inside a condition is an assignment that overwrites `x` and evaluates to `5` (true); `x == 5` is the comparison you almost always actually want. Watch compiler warnings for this mistake.
- `switch` compares one value against several constant cases; without an explicit `break`, execution falls through into the next case's code regardless of whether it matches — this is rarely intended and is a classic source of bugs.
- `switch` suits a single value against several exact constants (menus, single characters); `if`-`else` is required for ranges, multiple variables, or non-constant comparisons.

## Exercises

1. Write a program that reads an integer and prints whether it is positive, negative, or zero, using an `if`-`else if`-`else` ladder.
2. Write a program that reads three integers and prints the largest of the three, using nested `if` statements.
3. Rewrite Exercise 2 using only `if`-`else if`-`else` (no nesting), and compare which version you find easier to read.
4. Write a `switch`-based program that reads an integer from 1 to 7 and prints the corresponding day name, with a `default` case for any out-of-range input.
5. *(Harder)* Deliberately remove every `break` from your Exercise 4 program, run it for several different input values, and write down (as a comment at the top of the file) exactly what fall-through behavior you observe and why.
6. *(Harder)* Write a simple text-menu program (using `switch`) that presents options to convert a temperature: `1` for Celsius-to-Fahrenheit, `2` for Fahrenheit-to-Celsius, and anything else printing an error message via `default`. Handle the actual conversion arithmetic inside each `case`.

---
[← Chapter 5 — Input and Output](chapter-05.md) | [Chapter 7 — Loops and Iteration →](chapter-07.md)
