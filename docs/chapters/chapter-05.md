# Chapter 5 — Input and Output

## Introduction

So far, every program we've written has produced fixed, predetermined output. Real programs usually need to react to information that isn't known until the program is running — most commonly, input typed by the person using the program. This chapter covers C's standard tools for input and output (collectively often called **I/O**): `printf` in depth, `scanf` and its pitfalls, character-at-a-time I/O, and safe ways to read a full line of text.

## Standard Streams: stdin, stdout, stderr

A **stream** is an abstraction for a flow of data into or out of a program — you can think of it as a pipe through which bytes travel, without needing to know exactly where they originate or end up. C, via `<stdio.h>`, provides three standard streams that are automatically available to every program:

- **`stdin`** ("standard input") — where input normally comes from. By default, this is whatever you type at the terminal.
- **`stdout`** ("standard output") — where normal output normally goes. By default, this is the terminal screen. `printf` writes to `stdout`.
- **`stderr`** ("standard error") — a separate output stream specifically intended for error messages, kept distinct from `stdout` so that error text can be filtered, redirected, or displayed separately from normal program output, even though by default both `stdout` and `stderr` appear on the same terminal screen.

You don't need to do anything special to use these — they exist automatically the moment your program starts, which is why `printf("Hello\n")` (writing to `stdout`) has worked throughout this tutorial without any setup.

## `printf` Format Specifiers in Depth

`printf` takes a **format string** — the quoted text containing literal characters to print as-is, plus format specifiers (each starting with `%`) marking where, and how, to insert the values of the additional arguments that follow. Chapter 3 introduced a handful of these; here is a fuller picture.

| Specifier | Meaning |
|---|---|
| `%d` or `%i` | Signed `int`, printed in decimal. |
| `%u` | Unsigned `int`, printed in decimal. |
| `%f` | `float`/`double`, printed in fixed-point decimal notation. |
| `%lf` | Used with `scanf` for `double` (with `printf`, plain `%f` already handles `double`, since `float` arguments are automatically promoted). |
| `%c` | A single character. |
| `%s` | A string (a sequence of characters terminated by a special "end of string" marker, covered fully when we reach strings in a later chapter). |
| `%x` / `%X` | Unsigned integer in hexadecimal (base 16), lowercase or uppercase letters respectively. |
| `%o` | Unsigned integer in octal (base 8). |
| `%p` | A pointer value (an address in memory) — full meaning covered in Chapter 11. |
| `%%` | A literal percent sign; needed because a single `%` would otherwise start a format specifier. |

```c
#include <stdio.h>

int main(void) {
    int value = 255;

    printf("Decimal: %d\n", value);
    printf("Hexadecimal: %x\n", value);
    printf("Hexadecimal (uppercase): %X\n", value);
    printf("Octal: %o\n", value);
    printf("Character 'A': %c\n", 'A');
    printf("A literal percent sign: 100%%\n");

    return 0;
}
```

### Compile and run

```bash
gcc -std=c11 -Wall -Wextra formats.c -o formats
./formats
```

### Expected output

```text
Decimal: 255
Hexadecimal: ff
Hexadecimal (uppercase): FF
Octal: 377
Character 'A': A
A literal percent sign: 100%
```

### Width, Precision, and Flags

Between the `%` and the specifier letter, you can add extra instructions controlling exactly how the value is formatted:

- A **width** (a number) sets a minimum field width — the output is padded with spaces if it would otherwise be shorter. `%5d` means "print this integer in a field at least 5 characters wide."
- A **precision** (a number after a dot) controls, for `%f`, how many digits appear after the decimal point. `%.2f` means "exactly 2 digits after the decimal point."
- A **`-` flag** left-aligns the output within its field (the default is right-aligned). `%-10s` means "print this string left-aligned in a field at least 10 characters wide."
- A **`0` flag** pads with leading zeros instead of spaces, for numeric specifiers. `%05d` means "print this integer in a field at least 5 characters wide, padded with leading zeros."

```c
#include <stdio.h>

int main(void) {
    printf("[%5d]\n", 42);
    printf("[%-5d]\n", 42);
    printf("[%05d]\n", 42);
    printf("[%.2f]\n", 3.14159);
    printf("[%8.2f]\n", 3.14159);
    printf("[%-10s]\n", "hi");

    return 0;
}
```

### Compile and run

```bash
gcc -std=c11 -Wall -Wextra width_precision.c -o width_precision
./width_precision
```

### Expected output

```text
[   42]
[42   ]
[00042]
[3.14]
[    3.14]
[hi        ]
```

Read each line against the format used: `%5d` right-pads `42` with leading spaces to reach width 5; `%-5d` left-aligns it instead, with trailing spaces; `%05d` pads with zeros; `%.2f` rounds to 2 decimal digits; `%8.2f` combines a minimum width of 8 with 2 decimal digits of precision; `%-10s` left-aligns the string `"hi"` in a 10-character-wide field.

## `scanf`: Reading Input

`scanf` reads input from `stdin` according to a format string, working similarly to `printf` but in reverse — it parses text typed by the user and stores the resulting values into variables. Crucially, `scanf` needs to know the *memory address* of each variable it's writing into (not just its current value), so you must prefix each variable name with `&`, the **address-of operator**:

```c
#include <stdio.h>

int main(void) {
    int age;

    printf("Enter your age: ");
    scanf("%d", &age);
    printf("You are %d years old.\n", age);

    return 0;
}
```

### Compile and run

```bash
gcc -std=c11 -Wall -Wextra scanf_basic.c -o scanf_basic
./scanf_basic
```

### Example interactive session

```text
Enter your age: 30
You are 30 years old.
```

!!! note "Why `&`? A preview of pointers"
    `scanf` needs to *modify* the variable you pass it, but C normally passes values into functions as copies (full detail in Chapter 8 on functions). The `&` operator produces the variable's actual memory address, letting `scanf` reach into your program's memory and write the value directly into the right box, rather than into a copy of it. This address-based mechanism is the foundation of **pointers**, which is the entire subject of Chapter 11. For now, just remember the rule: when reading a value with `scanf` into an ordinary variable, write `&variable`.

### Pitfall 1: Leftover Newline in the Input Buffer

When you type a number and press Enter, the newline character you produced by pressing Enter is *not* consumed by `%d` — it stays waiting in the input buffer (the temporary holding area where typed input accumulates before your program reads it). If your very next read is `%c` (reading a single character), it will read that leftover newline instead of the character you intended to type next.

```c
#include <stdio.h>

int main(void) {
    int number;
    char letter;

    printf("Enter a number: ");
    scanf("%d", &number);

    printf("Enter a letter: ");
    scanf("%c", &letter);

    printf("Number: %d, Letter code: %d\n", number, letter);

    return 0;
}
```

### Compile and run

```bash
gcc -std=c11 -Wall -Wextra newline_bug.c -o newline_bug
./newline_bug
```

### Example interactive session (showing the bug)

```text
Enter a number: 5
Enter a letter: Number: 5, Letter code: 10
```

Notice the program never seemed to pause for the second input at all, and `Letter code: 10` is the numeric code for a newline character, not any letter you might have typed. This happened because after `scanf("%d", &number)` consumed the `5`, it left the newline (produced by pressing Enter) sitting in the buffer; the very next `scanf("%c", &letter)` then immediately read that leftover newline instead of waiting for you to type a letter.

**The fix** is to consume the leftover newline explicitly before reading the character, most simply with a space in the format string before `%c` (a space in a `scanf` format string tells it to skip any whitespace, including leftover newlines, before reading the next item):

```c
#include <stdio.h>

int main(void) {
    int number;
    char letter;

    printf("Enter a number: ");
    scanf("%d", &number);

    printf("Enter a letter: ");
    scanf(" %c", &letter);  // note the leading space before %c

    printf("Number: %d, Letter code: %d\n", number, letter);

    return 0;
}
```

### Compile and run

```bash
gcc -std=c11 -Wall -Wextra newline_fixed.c -o newline_fixed
./newline_fixed
```

### Example interactive session (fixed)

```text
Enter a number: 5
Enter a letter: q
Number: 5, Letter code: 113
```

### Pitfall 2: `scanf` Fails Silently on Bad Input

If you ask `scanf("%d", &number)` to read an integer but the user types something that isn't a number at all (like letters), `scanf` cannot perform the conversion. It leaves the variable's value unchanged (so it may still contain whatever leftover/indeterminate value it had before, per Chapter 3) and leaves the bad input sitting in the buffer — but critically, your program does not automatically know this happened unless you check `scanf`'s **return value**. `scanf` returns the number of items it successfully read; checking that return value is the only reliable way to detect bad input.

```c
#include <stdio.h>

int main(void) {
    int number;

    printf("Enter a number: ");
    int items_read = scanf("%d", &number);

    if (items_read == 1) {
        printf("You entered: %d\n", number);
    } else {
        printf("That wasn't a valid number.\n");
    }

    return 0;
}
```

### Compile and run

```bash
gcc -std=c11 -Wall -Wextra scanf_check.c -o scanf_check
./scanf_check
```

### Example interactive session (bad input)

```text
Enter a number: abc
That wasn't a valid number.
```

Without checking `items_read`, the program would have silently continued with whatever indeterminate value `number` already had, which is a much harder bug to notice and track down than an explicit message.

### Pitfall 3: Buffer Overflow Risk with `%s`

When reading text into a fixed-size array of characters (we'll cover arrays formally in a later chapter; for now, think of it as a fixed-length row of boxes for characters) using `%s` without a width limit, `scanf` will keep writing characters for as long as the user keeps typing, with no awareness of how big the destination actually is. If the user types more characters than the array can hold, `scanf` will write past the end of it — a **buffer overflow**. This corrupts whatever memory happens to sit just after the array, which can crash the program or, in more serious real-world cases, be exploited as a security vulnerability.

```c
#include <stdio.h>

int main(void) {
    char name[10]; // room for only 9 characters plus a terminator

    printf("Enter your name: ");
    scanf("%9s", name); // width limit prevents overflow: at most 9 characters
    printf("Hello, %s!\n", name);

    return 0;
}
```

### Compile and run

```bash
gcc -std=c11 -Wall -Wextra scanf_width.c -o scanf_width
./scanf_width
```

### Example interactive session

```text
Enter your name: Alexandra
Hello, Alexandra!
```

The width limit `%9s` (one less than the array's size of 10, leaving room for the automatic string terminator) ensures `scanf` will never write more characters into `name` than it can safely hold, no matter how much the user types. Always include an explicit width limit whenever you use `%s` with a fixed-size destination.

## `getchar()` and `putchar()`

`getchar()` reads a single character from `stdin` and returns it; `putchar()` writes a single character to `stdout`. These are the simplest possible I/O functions, useful when you want to process input one character at a time.

```c
#include <stdio.h>

int main(void) {
    printf("Type a character: ");
    int ch = getchar();

    printf("You typed: ");
    putchar(ch);
    putchar('\n');

    return 0;
}
```

### Compile and run

```bash
gcc -std=c11 -Wall -Wextra getchar_demo.c -o getchar_demo
./getchar_demo
```

### Example interactive session

```text
Type a character: Q
You typed: Q
```

!!! note "Why does `getchar()` return an `int`, not a `char`?"
    `getchar()`'s return type is `int`, not `char`, so that it has room for one extra special value, `EOF` ("end of file"), distinct from every possible real character value, used to signal that there is no more input to read. This detail matters more once we cover reading input in loops and from files in later chapters.

## Why `gets()` Is Dangerous and `fgets()` Is the Safe Replacement

An older function, `gets()`, used to be a common way to read a full line of text into a character array. `gets()` has no way to know how big the destination array is — it simply keeps reading and writing characters until it hits a newline, with absolutely no bounds checking. This makes `gets()` a guaranteed buffer overflow risk for any input longer than the array, and it was so dangerous that it was formally removed from the C standard library entirely (in the C11 standard). **You should never use `gets()`** — modern compilers will typically refuse to compile code that uses it, or at minimum issue a strong warning/deprecation notice.

The safe replacement is **`fgets()`**, which requires you to explicitly state the maximum number of characters to read, so it can never write past the end of your array:

```c
#include <stdio.h>

int main(void) {
    char line[50];

    printf("Enter a sentence: ");
    fgets(line, sizeof(line), stdin);

    printf("You entered: %s", line);

    return 0;
}
```

### Compile and run

```bash
gcc -std=c11 -Wall -Wextra fgets_demo.c -o fgets_demo
./fgets_demo
```

### Example interactive session

```text
Enter a sentence: Hello there, this is a test.
You entered: Hello there, this is a test.
```

`fgets(line, sizeof(line), stdin)` reads from `stdin` into the array `line`, never writing more than `sizeof(line)` bytes in total (using `sizeof` here, as introduced in Chapter 3, conveniently and automatically matches the limit to the array's actual declared size — much safer than writing a fixed number by hand, which could become wrong if you later resize the array). Unlike `scanf("%s", ...)`, `fgets` also reads the newline character itself into the array (if there's room), and it can read an entire line including spaces, where `scanf("%s", ...)` would have stopped at the first space.

## A Worked Example: Combining Everything

Let's combine `printf`, `scanf`, format specifiers, and the newline pitfall (with its fix) into one small, realistic program: asking for a name, age, and favorite number, then printing a formatted summary.

First, here is the version **with the newline bug**, so you can see it happen in a more realistic context:

```c
#include <stdio.h>

int main(void) {
    char initial;
    int age;
    double favorite_number;

    printf("Enter the first letter of your name: ");
    scanf("%c", &initial);

    printf("Enter your age: ");
    scanf("%d", &age);

    printf("Enter your favorite number: ");
    scanf("%lf", &favorite_number);

    printf("\n--- Summary ---\n");
    printf("Initial: %c\n", initial);
    printf("Age: %d\n", age);
    printf("Favorite number: %.2f\n", favorite_number);

    return 0;
}
```

### Compile and run

```bash
gcc -std=c11 -Wall -Wextra summary_buggy.c -o summary_buggy
./summary_buggy
```

### Example interactive session (showing the bug)

```text
Enter the first letter of your name: A
Enter your age: 
```

After typing `A` and pressing Enter, the very next `scanf("%d", &age)` immediately reads the leftover newline as part of its whitespace-skipping behavior for numeric conversions (numeric `scanf` conversions, unlike `%c`, automatically skip leading whitespace, so this particular pairing actually works correctly) — but if you reorder things so a `%c` read follows a `%d` read, exactly as in the earlier Pitfall 1 example, the bug reappears. To make this concrete and consistently demonstrate the issue, let's read the age first, then the initial, mirroring Pitfall 1 exactly:

```c
#include <stdio.h>

int main(void) {
    int age;
    char initial;
    double favorite_number;

    printf("Enter your age: ");
    scanf("%d", &age);

    printf("Enter the first letter of your name: ");
    scanf("%c", &initial);  // bug: reads leftover newline, not a real letter

    printf("Enter your favorite number: ");
    scanf("%lf", &favorite_number);

    printf("\n--- Summary ---\n");
    printf("Initial: %c\n", initial);
    printf("Age: %d\n", age);
    printf("Favorite number: %.2f\n", favorite_number);

    return 0;
}
```

### Compile and run

```bash
gcc -std=c11 -Wall -Wextra summary_buggy2.c -o summary_buggy2
./summary_buggy2
```

### Example interactive session (bug reproduced)

```text
Enter your age: 30
Enter the first letter of your name: Enter your favorite number: 7.5

--- Summary ---
Initial: 
Age: 30
Favorite number: 7.50
```

As predicted, the program never actually paused to let you type a name initial — it silently consumed the leftover newline from the age input, and the printed "Initial" line is blank. Now here is the **corrected** version, using a leading space before `%c` (Pitfall 1's fix), `scanf`'s return value check (Pitfall 2's fix), and proper format specifiers throughout:

```c
#include <stdio.h>

int main(void) {
    int age;
    char initial;
    double favorite_number;

    printf("Enter your age: ");
    if (scanf("%d", &age) != 1) {
        printf("Invalid age input.\n");
        return 1;
    }

    printf("Enter the first letter of your name: ");
    scanf(" %c", &initial); // leading space skips the leftover newline

    printf("Enter your favorite number: ");
    scanf("%lf", &favorite_number);

    printf("\n--- Summary ---\n");
    printf("Initial: %c\n", initial);
    printf("Age: %d\n", age);
    printf("Favorite number: %.2f\n", favorite_number);

    return 0;
}
```

### Compile and run

```bash
gcc -std=c11 -Wall -Wextra summary_fixed.c -o summary_fixed
./summary_fixed
```

### Example interactive session (fixed)

```text
Enter your age: 30
Enter the first letter of your name: A
Enter your favorite number: 7.5

--- Summary ---
Initial: A
Age: 30
Favorite number: 7.50
```

Notice the program now correctly pauses for, and correctly reads, all three pieces of input, and the `if (scanf(...) != 1)` check guards against invalid age input by returning `1` early (recall from Chapter 2 that a non-zero return value from `main` conventionally signals failure).

## Summary

- `stdin`, `stdout`, and `stderr` are the three standard streams every C program has automatically: input, normal output, and error output respectively.
- `printf` format specifiers cover decimal (`%d`/`%i`/`%u`), floating-point (`%f`), characters (`%c`), strings (`%s`), hex/octal (`%x`/`%X`/`%o`), pointers (`%p`), and a literal percent (`%%`); width, precision, and flags (`%5d`, `%.2f`, `%-10s`, `%05d`) control formatting in detail.
- `scanf` requires `&variable` (the address-of operator) to write into ordinary variables; this is a first preview of pointers, covered fully in Chapter 11.
- Three key `scanf` pitfalls: a leftover newline can be misread by a following `%c` (fix: a leading space in the format string); failing to check `scanf`'s return value hides bad/non-numeric input; and `%s` without an explicit width limit on a fixed-size array risks a buffer overflow.
- `getchar()`/`putchar()` read/write one character at a time; `getchar()` returns `int` (not `char`) to make room for the special `EOF` value.
- `gets()` was removed from the C standard because it has no bounds checking; `fgets(buffer, sizeof(buffer), stdin)` is the safe, standard replacement for reading a full line.

## Exercises

1. Write a program that reads two integers with `scanf` and prints their sum, difference, product, and integer quotient, each labeled clearly.
2. Write a program that uses `%8.3f` to print three different floating-point numbers of your choosing, and explain in a comment what the `8` and the `3` each control.
3. Reproduce the leftover-newline bug from this chapter yourself (read an `int`, then read a `char` with plain `%c`), confirm you see the bug, then fix it using the leading-space technique.
4. Write a program that reads a name into a fixed-size `char` array of size 6 using `fgets`, and test it by entering a name longer than 5 characters. Confirm the program does not crash or overflow, and explain, referencing `sizeof`, why it's safe.
5. *(Harder)* Write a program that reads an integer with `scanf`, checks the return value, and loops (you can use a simple `while` here even though loops are formally covered in Chapter 6 — a `while (condition) { ... }` repeats its body while `condition` is true) asking the user to re-enter the value until valid input is given, being careful to clear any leftover bad input from the buffer using repeated `getchar()` calls until a newline is consumed.
6. *(Harder)* Modify the final "Worked Example" fixed program so that it also reads a full favorite color as a line of text (using `fgets`) after the favorite number, and prints it in the summary. Think carefully about leftover newlines from the preceding `%lf` read interacting with `fgets`, test your program, and fix any unexpected behavior you observe.

---
[← Chapter 4 — Operators and Expressions](chapter-04.md)
