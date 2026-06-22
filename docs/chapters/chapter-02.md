# Chapter 2 — Your First C Program and the Compilation Process

## Introduction

In Chapter 1 you installed a compiler and confirmed it works by compiling a one-line test program, without us explaining what that program meant. Now we slow down and explain everything: every character of a complete "Hello, World!" program, what actually happens inside gcc when you compile, how to read and fix compiler errors and warnings, and the basic grammar rules of C syntax.

## Hello, World! — Line by Line

Here is the complete program again, in full:

```c
#include <stdio.h>

int main(void) {
    printf("Hello, world!\n");
    return 0;
}
```

### Compile and run

```bash
gcc hello.c -o hello
./hello
```

### Expected output

```text
Hello, world!
```

Now let's go through it piece by piece.

**`#include <stdio.h>`**

This line is a **preprocessor directive** — an instruction that runs *before* the actual compilation step, processed by a part of gcc called the preprocessor. `#include` means "copy the contents of another file in here, right at this spot, before compiling." `<stdio.h>` is the **standard input/output header** — a file, provided as part of C's standard library, that contains the declarations needed to use functions like `printf` (which prints text to the screen). Without this line, the compiler would have no idea what `printf` is, since `printf` is not a built-in part of the C language itself — it is a function provided by the standard library, and you have to explicitly tell the compiler you intend to use it. We will explain the preprocessor in full depth in Chapter 15; for now, just remember: "if you want to use a standard library feature, `#include` the header that provides it."

**`int main(void)`**

Every C program must have exactly one function named `main` — this is the designated **entry point**: the first piece of your own code that runs when the program starts. (Functions in general are covered fully in Chapter 8; for now, think of a function as a named, reusable block of instructions.) The word `int` before `main` says that this function will report back a whole number (an integer) when it finishes — by convention, this number tells whatever launched the program (like your terminal) whether the program succeeded or failed. The `(void)` says that `main`, in this version, does not accept any input values when it's started. (We'll see a different form of `main` that *does* accept input, in later chapters, once we've covered arrays.)

**`{` and `}`**

These curly braces mark the beginning and end of a **block** — a group of statements that belong together. Here, they mark "everything between these braces is the body of the `main` function: the instructions that run when the program starts." Every opening brace `{` must have a matching closing brace `}`.

**`printf("Hello, world!\n");`**

This line does the actual work. `printf` is a function (provided by `<stdio.h>`) that prints text to the screen. The text to print is given in double quotes: `"Hello, world!\n"`. This quoted text is called a **string literal**. Inside it, `\n` is a special two-character sequence representing a single **newline** character — it moves the cursor to the start of the next line, the same way pressing Enter would. We'll cover other such escape sequences and string formatting in much more depth in Chapter 5.

**`;` (the semicolon)**

Every individual instruction in C — called a **statement** — must end with a semicolon. This tells the compiler "this statement is complete." Forgetting it is one of the most common beginner mistakes, and we'll deliberately trigger that exact error in a moment so you know what it looks like.

**`return 0;`**

This ends the `main` function and reports the value `0` back to whatever launched the program. By long-standing convention in C (and inherited by many other languages), a return value of `0` means "the program finished successfully," and any non-zero value signals some kind of failure or error condition. You, the programmer, choose what value to return — `0` for success is a convention, not a rule enforced by the language, but it is so universal that you should always follow it for normal, successful completion.

## The Four Stages of Compilation

When you run `gcc hello.c -o hello`, it looks like one single step, but internally gcc performs four distinct stages, each transforming the program into a different representation, before you get a runnable executable. Understanding these stages will make compiler error messages far less mysterious.

```text
hello.c  --[1. Preprocessing]-->  expanded source
         --[2. Compilation]----->  assembly code
         --[3. Assembly]--------->  machine code (object file)
         --[4. Linking]---------->  hello (executable)
```

**Stage 1 — Preprocessing.** The preprocessor handles every line starting with `#`, including `#include`. It literally copies the contents of `stdio.h` (and anything else included) into your file, textually, before any real compilation happens. The output of this stage is a single, much larger block of pure C source text, with no `#include` lines left — they've all been expanded in place. (Other `#` directives, like `#define`, are also handled here — full detail in Chapter 15.)

**Stage 2 — Compilation.** This is the stage most people mean when they loosely say "compiling." The compiler reads the preprocessed C source and translates it into **assembly language** — a human-readable (but extremely low-level) text representation of CPU instructions, specific to your computer's processor type. This is also the stage where the compiler checks your code for correctness against C's grammar rules, which is why most errors and warnings are reported here.

**Stage 3 — Assembly.** A program called an **assembler** translates the assembly language text from Stage 2 into actual machine code — raw binary instructions — and packages it into what's called an **object file** (often ending in `.o`). An object file contains machine code, but it is not yet a complete, runnable program if your code refers to functions (like `printf`) defined elsewhere.

**Stage 4 — Linking.** A program called the **linker** takes your object file(s) and combines them with the object code for any library functions you used (like `printf`'s actual implementation, which lives in the C standard library) into one single, self-contained executable file. This resolves all the "where is the actual code for `printf`?" references, producing the final runnable program — `hello` in our example.

When you type the single command `gcc hello.c -o hello`, gcc quietly drives all four of these stages for you. Later in this tutorial (Chapter 17 and the Appendix) we will look at multi-file projects, where understanding the line between compilation and linking becomes directly useful, since errors at each stage look and feel different.

## Compiling with gcc: Useful Flags

The basic compile command has this shape:

```bash
gcc hello.c -o hello
```

`gcc` is the command. `hello.c` is your source file. `-o hello` tells gcc to name the output executable `hello` (without `-o`, gcc would name it `a.out` by default, which is a much less useful name).

Here are flags you should make a habit of using:

| Flag | Meaning |
|---|---|
| `-o <name>` | Name the output executable `<name>` instead of the default `a.out`. |
| `-Wall` | Enable "all" the commonly recommended warnings. Catches a large class of likely mistakes. |
| `-Wextra` | Enable additional warnings not included in `-Wall`. Use together with `-Wall`. |
| `-g` | Include debugging information in the executable, so tools like `gdb` can map machine instructions back to your source lines. |
| `-std=c11` | Compile according to the C11 standard specifically (rather than whatever gcc's default happens to be), ensuring consistent, predictable behavior. |

A solid, recommended everyday compile command for this entire tutorial is:

```bash
gcc -std=c11 -Wall -Wextra -g hello.c -o hello
```

This compiles strictly to the C11 standard, turns on thorough warnings, and includes debug information. We will use warnings extensively in this chapter to learn how to read compiler feedback.

### Running the executable

On Linux and macOS (including WSL):

```bash
./hello
```

On native Windows (MSYS2), the produced file will be named `hello.exe`, and you run it the same way:

```bash
./hello.exe
```

(Double-clicking the `.exe` from a file browser also works on Windows, but using the terminal, as shown, keeps your workflow consistent with the rest of this tutorial, and lets you actually see printed output.)

## Reading and Fixing Compiler Feedback

Compiler messages can look intimidating at first, but they almost always tell you exactly the file, line number, and nature of the problem. Let's deliberately break our program twice and walk through fixing it.

### A compiler error: the missing semicolon

Take the working program and remove the semicolon after `printf`:

```c
#include <stdio.h>

int main(void) {
    printf("Hello, world!\n")
    return 0;
}
```

Compile it:

```bash
gcc -std=c11 -Wall -Wextra hello.c -o hello
```

You will see an error similar to:

```text
hello.c: In function 'main':
hello.c:5:5: error: expected ';' before 'return'
    5 |     return 0;
      |     ^~~~~~
```

Read this carefully: it names the file (`hello.c`), the function it occurred in (`main`), and — crucially — `hello.c:5:5`, meaning line 5, column 5. The message `expected ';' before 'return'` tells you the compiler expected to find a semicolon somewhere before it reached the word `return`. Notice the error is reported on the line *after* the actual mistake — this is normal and extremely common: the compiler doesn't realize a statement is incomplete until it starts reading the *next* one. The fix is to go back to the previous line (line 4, the `printf` line) and add the missing semicolon:

```c
#include <stdio.h>

int main(void) {
    printf("Hello, world!\n");
    return 0;
}
```

Recompiling now produces no output at all from gcc — meaning success.

!!! tip "Reading error locations"
    When a compiler error mentions a line number, always check that line *and the one immediately before it*. Many syntax errors (especially missing semicolons and unmatched braces) are only detected one token after the actual mistake.

### A compiler warning: mismatched format type

Now let's trigger a *warning* rather than a hard error. Warnings let the program compile anyway, but they flag something suspicious that is very likely a real bug. Consider this version, which tries to print a floating-point number (we'll cover this type fully in Chapter 3) using the wrong format specifier:

```c
#include <stdio.h>

int main(void) {
    float price = 9.99f;
    printf("Price: %d\n", price);
    return 0;
}
```

Compile with warnings enabled:

```bash
gcc -std=c11 -Wall -Wextra hello.c -o hello
```

You'll see something like:

```text
hello.c:5:19: warning: format '%d' expects argument of type 'int',
but argument 2 has type 'double' [-Wformat=]
    5 |     printf("Price: %d\n", price);
      |                    ~^     ~~~~~
      |                     |      |
      |                     int    double
```

This warning is telling you that `%d` (the format specifier for printing an integer — covered fully in Chapters 3 and 5) does not match the actual type of `price`, which is a floating-point value. Despite the warning, gcc still produces an executable, and running it will print garbage-looking output, because `printf` is being told to interpret the underlying bits as an integer when they actually represent a floating-point value. The fix is to use the correct format specifier, `%f`, for floating-point values:

```c
#include <stdio.h>

int main(void) {
    float price = 9.99f;
    printf("Price: %f\n", price);
    return 0;
}
```

### Compile and run

```bash
gcc -std=c11 -Wall -Wextra hello.c -o hello
./hello
```

### Expected output

```text
Price: 9.990000
```

(Don't worry about the trailing zeros — we'll explain precision control for `%f` in Chapter 5.)

!!! warning "Never ignore warnings"
    A program that compiles "successfully" despite warnings is not the same as a program that is correct. Warnings exist precisely because the compiler has noticed something that compiles, but is probably not what you meant. Build the habit, starting now, of compiling with `-Wall -Wextra` every time and treating every warning as a bug report to investigate.

## Comments

A **comment** is text in your source file that the compiler completely ignores — it exists purely for human readers. C provides two styles.

**Single-line comments**, starting with `//` and running to the end of the line:

```c
#include <stdio.h>

int main(void) {
    // Tell the user the program started successfully.
    printf("Hello, world!\n");
    return 0;
}
```

**Multi-line (block) comments**, starting with `/*` and ending with `*/`, which can span several lines:

```c
#include <stdio.h>

/*
 * This program is our very first C program.
 * It simply prints a greeting to the screen.
 */
int main(void) {
    printf("Hello, world!\n");
    return 0;
}
```

Good comments explain *why* something is done a certain way, not *what* the code literally does — the code itself already shows "what." For example, a comment like `// add 1 to x` above the line `x = x + 1;` adds no value, since the code already says that plainly. A comment like `// adjust for the off-by-one from the legacy file format` is valuable, because it tells you something the code alone can't: the reasoning behind it. Use comments sparingly and purposefully — too many low-value comments clutter code and become outdated when the code changes but the comment doesn't.

## Basic Syntax Rules

A few foundational grammar rules apply to every C program you will ever write:

**C is case-sensitive.** `main`, `Main`, and `MAIN` are three completely different names as far as the compiler is concerned. The entry point function must be written exactly as `main`, all lowercase.

**Whitespace is mostly free-form.** Spaces, tabs, and blank lines (collectively, **whitespace**) are, with few exceptions, not meaningful to the compiler — they exist for human readability. This program:

```c
#include <stdio.h>
int main(void){printf("Hi\n");return 0;}
```

compiles identically to our nicely formatted version. The compiler does not care about indentation or line breaks here; *you* care, because readable formatting helps humans understand the code. This tutorial will always use consistent indentation (4 spaces per level), and you should adopt a consistent style too.

**Every statement ends with a semicolon.** We saw this above — it's how the compiler knows where one instruction ends and the next begins.

**Braces are required for function bodies, but optional for single-statement blocks.** We saw `{ }` required around the entire body of `main`. Later, starting in Chapter 6, you'll learn about `if` statements and loops, which technically allow you to omit braces if their body is a single statement:

```c
#include <stdio.h>

int main(void) {
    int x = 5;
    if (x > 0)
        printf("Positive\n");
    return 0;
}
```

This compiles and runs correctly. However, this tutorial strongly recommends *always* using braces, even for single statements, because omitting them is a well-known source of subtle bugs when code is edited later and a second statement is added without noticing it falls outside the (invisible) single-statement block. We will revisit this recommendation when we cover control flow properly in Chapter 6.

## Summary

- `#include <stdio.h>` copies in declarations needed for standard I/O functions like `printf`; full preprocessor detail comes in Chapter 15.
- `int main(void) { ... }` defines the program's entry point; `return 0;` signals successful completion by convention.
- Compilation happens in four stages: preprocessing (handles `#` directives), compilation (source to assembly, where most errors/warnings are caught), assembly (assembly to machine code/object file), and linking (combines object code into a final executable).
- Compile with `gcc -std=c11 -Wall -Wextra -g file.c -o output`, then run with `./output` (or `./output.exe` on native Windows).
- Compiler errors prevent an executable from being produced and must be fixed; warnings allow compilation but flag likely bugs and should never be ignored.
- Error messages report a file, line, and column — but the true mistake is sometimes on the line just before the reported one.
- Use `//` for single-line comments and `/* ... */` for multi-line comments, sparingly, to explain *why*, not *what*.
- C is case-sensitive, whitespace is mostly free-form (for human readability only), every statement needs a semicolon, and braces — while sometimes optional around single statements — should be used consistently for safety.

## Exercises

1. Write a complete C program that prints your name followed by a newline. Compile it with `-Wall -Wextra` and run it.
2. Take the program from Exercise 1 and remove the closing brace `}` at the end of `main`. Compile it and read the resulting error message carefully — what line does it report, and why might that be different from the line you actually changed?
3. Write a short program declaring a `float` variable and deliberately printing it with `%d` (as shown in this chapter) to reproduce the format-mismatch warning yourself, then fix it.
4. Add both a `//` comment and a `/* ... */` comment to one of your programs, explaining the *purpose* of the `printf` line (not merely restating what it does).
5. *(Harder)* Predict, without running it, what happens if you compile a `.c` file that contains only `#include <stdio.h>` and nothing else — no `main` function at all. Then try it and compare the actual compiler/linker message to your prediction. Which of the four compilation stages do you think is reporting the problem?
6. *(Harder)* Rewrite the "Hello, world!" program entirely on one line, with no extra whitespace at all (similar to the compact example shown in the Basic Syntax Rules section), and confirm it still compiles and runs identically. Then explain, in your own words, why this works given what you now know about whitespace.

---
[← Chapter 1 — Setting Up Your Environment](chapter-01.md) | [Chapter 3 — Variables, Data Types and Constants →](chapter-03.md)
