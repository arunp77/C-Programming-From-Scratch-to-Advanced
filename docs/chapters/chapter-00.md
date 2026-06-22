# Chapter 0 — Introduction to Programming and C

## Welcome

This is the first page of a tutorial that assumes nothing. If you have never written a single line of code in your life, never opened a terminal, and have only a vague idea of what "programming" means, you are exactly the reader this tutorial is written for.

We are going to learn the C programming language from the very first principles: what a program actually is, how to set up the tools to write one, and then, step by step, how to read and write C code with confidence. By the end, you will understand not just C syntax, but many of the underlying ideas (memory, the CPU, compilation) that make all other programming languages work the way they do.

## What Is a Computer Program?

Before we talk about C, we need to talk about what a computer actually does.

A computer's processor — the **CPU** (Central Processing Unit) — is, at its core, an extremely fast but extremely literal-minded machine. It can only do very simple things: add two numbers, compare two numbers, move a piece of data from one place to another, and decide "if this condition is true, jump to a different instruction." That is essentially it. The CPU has no concept of "calculate my tax return" or "show me a cat video." It only understands a long list of tiny, primitive instructions, executed one at a time, in order (with some exceptions for jumps and branches, which we'll get to in later chapters).

A **program** is simply a list of these instructions, written down in advance, that tells the CPU exactly what to do, one step after another, to accomplish some larger goal. Think of it like a recipe: a recipe is a list of small, unambiguous steps ("crack two eggs into a bowl," "whisk for 30 seconds," "pour into the pan") that, followed in order, produce a cake. A computer program is a "recipe" for the CPU, except the steps are things like "store the number 5 in this location," "add the number stored at location A to the number stored at location B," or "if the result is greater than 10, skip the next instruction."

The raw form of these instructions — the form the CPU actually understands — is called **machine code**: streams of numbers (which are themselves represented as patterns of electrical signals, conventionally written as 1s and 0s, called **binary**) that correspond directly to specific CPU operations. Every running program on your computer, no matter what language it was originally written in, eventually becomes machine code before the CPU executes it.

!!! note "Mental model: memory as labeled boxes"
    Throughout this tutorial we will repeatedly come back to one mental picture: a computer's memory (**RAM**) is like a long row of labeled boxes, each capable of holding a small piece of data. A program's job is to put values into boxes, take values out of boxes, and combine the values in useful ways. "Writing a program" means writing down, precisely, which boxes to use and what to do with their contents. We will make this picture much more concrete in Chapter 3 when we introduce variables.

## Why Do We Need a "Programming Language"?

If the CPU only understands machine code — raw numbers representing primitive instructions — why don't we just write programs directly in machine code?

In the very earliest days of computing, people did exactly that. It was miserable. Machine code is:

- **Unreadable to humans.** A list of numbers like `10110000 01100001` tells you nothing about what the program is trying to accomplish.
- **Error-prone.** A single wrong digit can corrupt the entire program, and there is no helpful error message — the computer will simply do the wrong thing, or crash.
- **Tied to one specific CPU design.** Different CPU families understand different machine code. A program written in raw machine code for one type of processor generally will not run at all on a different type of processor.

A **programming language** is a human-readable notation for describing what we want the computer to do. Instead of memorizing strings of binary digits, we write things like:

```text
x = 5
y = 10
total = x + y
```

This is vastly easier for a human to write, read, and fix. But the CPU still cannot understand this text directly — it only understands machine code. So we need something that bridges the gap between the language we write in and the machine code the CPU executes. That bridge is either a **compiler** or an **interpreter**, and the distinction between them is one of the most important conceptual ideas in all of programming.

## Compiled vs. Interpreted Languages (the Big Picture)

At a conceptual level, there are two broad strategies for getting from human-readable source code to something a CPU can run.

**Compiled languages.** A program called a **compiler** reads your entire source code file, translates it all at once into machine code, and produces a new file — an **executable** — containing that machine code. After that, you run the executable directly; the compiler is no longer involved. C is a compiled language. We will look at exactly how this works, in detail, in Chapter 2.

**Interpreted languages.** A program called an **interpreter** reads your source code and executes it line by line, on the fly, translating and running each piece as it goes, without ever producing a separate, reusable file of pure machine code. Python and JavaScript are common examples of languages that are typically run this way (though the real-world details are often more nuanced — many "interpreted" languages use tricks that blur this line, but that nuance is beyond what you need right now).

The practical difference you will feel as a learner: with C, you write code, then run a separate **compile** step that checks your code and turns it into a runnable program, and only then do you execute that program. If you make a typo, the compiler will typically refuse to produce a program at all, and will tell you what's wrong. This "compile, then run" rhythm will feel new if you have never programmed before, but it quickly becomes second nature, and it is one of the reasons C programs tend to run extremely fast: all the translation work happens once, ahead of time, rather than repeatedly while the program runs.

## A Tiny Bit of History

C was created in 1972 by **Dennis Ritchie** at **Bell Labs**, the research arm of the American telephone company AT&T. Ritchie and his colleague **Ken Thompson** were building an operating system called **Unix** — software that manages a computer's hardware and lets other programs run on it. (Modern Linux and macOS both trace deep conceptual and historical roots back to Unix.)

The earliest versions of Unix had been written in assembly language, a very low-level notation tied tightly to one specific type of CPU. This made Unix difficult to maintain and impossible to move to a different type of computer without rewriting huge portions of it from scratch. Ritchie and Thompson wanted a language that was powerful and efficient enough to write an operating system in — something that could manipulate memory and hardware directly — but portable enough that the same source code could be recompiled for different machines with minimal changes.

C was the result. It struck an unusual balance: it gives the programmer very fine, direct control over memory and hardware (similar to assembly language), while still being a readable, structured, human-friendly language with named variables, functions, loops, and so on. This combination was, at the time, genuinely new, and it proved enormously influential. Unix itself was rewritten in C shortly after, which made it dramatically easier to port to new machines — and that portability is a large part of why Unix (and its descendants) spread so widely through universities and industry over the following decades.

## Why C Still Matters Today

C is over fifty years old, and yet it is still one of the most widely used and important programming languages in the world. Here is why.

**Operating system kernels.** The core of Linux, the core of Windows, and large parts of macOS's underlying system (which descends from Unix) are written substantially in C. When you need a language that can talk almost directly to hardware while remaining manageable for large engineering teams, C remains the default choice.

**Embedded systems.** Embedded systems are small, often resource-constrained computers built into other devices: microwave ovens, cars, medical devices, industrial sensors, routers, and so on. These systems frequently have very little memory and need extremely predictable, efficient code. C's combination of low-level control and minimal overhead makes it the dominant language in this space.

**Performance-critical software.** Anywhere that speed and tight control over memory matter — game engines, database engines, scientific computing, real-time audio/video processing — C (and its close relative C++) remains a top choice, because C does not impose hidden costs that the programmer can't see or control.

**It underlies other languages.** This is a point worth sitting with: many of the programming languages you may have heard of — Python, Ruby, PHP, and parts of JavaScript's engines, among others — have their core **runtime** (the underlying software that actually executes the language) implemented in C. Even languages that don't use C internally were often *influenced* by C's syntax — the curly-brace, semicolon-terminated style you'll learn in this tutorial shows up, with variations, in C++, Java, JavaScript, C#, Go, Rust, and more. Learning C gives you a mental model that transfers to an enormous fraction of all the software you will ever encounter.

!!! tip "You are learning a foundation, not just a language"
    Because so many later languages borrowed C's ideas and syntax, what you learn here — variables, types, control flow, functions, memory — is not a niche skill. It is the shared vocabulary underneath a huge portion of modern computing.

## What You Will Be Able to Build by the End

By the time you finish this tutorial, you will be able to:

- Read and write complete C programs from scratch, using variables, arithmetic, decision-making (if/else), and repetition (loops).
- Organize code into reusable functions, and understand exactly how data moves into and out of them.
- Understand and use pointers and memory addresses — the feature most associated with C's power (and its danger).
- Work with arrays and strings, and understand how C represents text under the hood.
- Manage your own memory manually (allocating and freeing it), and understand the responsibilities that come with that power.
- Define your own custom data types using structures, and build small data structures like linked lists.
- Read and write files.
- Understand the preprocessor and the build process well enough to organize a multi-file C project.
- Recognize and avoid the most common categories of bugs that make C notoriously tricky: undefined behavior, buffer overflows, and memory leaks.

## A Teaser (Don't Worry About Understanding This Yet)

Just so you have seen actual C code before we dive into setup and tooling, here is a complete, tiny C program. We are not going to explain it yet — every single piece of this will be explained carefully starting in Chapter 2. For now, just notice that it is short, uses English words, and is far more readable than raw machine code.

```c
#include <stdio.h>

int main(void) {
    printf("Hello, world!\n");
    return 0;
}
```

If you ran this program, it would print the text `Hello, world!` to your screen. That's all it does. Don't worry about understanding `#include`, `int main(void)`, or `printf` right now — by the end of Chapter 2, you will be able to explain every character of this program to someone else.

## How This Tutorial Is Organized

This tutorial is organized into broad parts, each building on the last. Here is the map:

1. **Getting Started** (Chapters 0–2): What programming and C are, setting up your tools, and writing/compiling your first program.
2. **Language Basics** (Chapters 3–5): Variables, types, operators, and basic input/output — the vocabulary and grammar of C.
3. **Core Concepts** (Chapters 6–9, covered later): Control flow (if/else, loops), functions, arrays, and strings.
4. **Intermediate Topics** (covered later): Pointers, dynamic memory allocation, and structures.
5. **Advanced Topics** (covered later): File input/output, the preprocessor in depth, multi-file projects, and an introduction to writing safer, more robust C.
6. **Appendix**: Reference material — operator precedence tables, common standard library functions, and further reading.

Each chapter builds directly on the ones before it, and later chapters will sometimes briefly mention a concept ("we'll explain this fully in Chapter X") before it is formally introduced, so that you understand why something is written the way it is without getting a full explanation before you're ready for it.

## Summary

- A computer program is a precise list of simple instructions for the CPU, executed one at a time.
- The CPU only understands machine code (raw binary instructions); programming languages exist so humans can write readable instructions instead.
- A compiler translates an entire program's source code into machine code ahead of time, producing an executable file; an interpreter translates and runs code line by line as it goes. C is a compiled language.
- C was created in 1972 by Dennis Ritchie at Bell Labs, originally to rewrite the Unix operating system in a portable, maintainable way.
- C remains hugely important today because of operating system kernels, embedded systems, performance-critical software, and because it underlies the runtimes of many other popular languages.
- This tutorial proceeds from absolute basics (setup, first program) through language fundamentals, core control structures, and on to intermediate and advanced topics like pointers, memory management, and file I/O.

## Exercises

1. In your own words, explain the difference between a "program" and a "programming language."
2. Why can't a CPU run C source code directly? What has to happen first?
3. Name two reasons C is still used today, more than fifty years after it was created.
4. Look up (using any search engine) one well-known piece of software that is written substantially in C. Write one sentence about what it does.
5. Without worrying about syntax, describe in plain English what you think the teaser program in this chapter does, based only on the explanation given.
6. *(Harder)* Compiled and interpreted languages each have trade-offs. Based only on the conceptual descriptions in this chapter, suggest one advantage an interpreted language might have over a compiled one, and one advantage a compiled language might have over an interpreted one. (Hint: think about how quickly you can test a one-line change, versus how fast the final program runs.)

---
[Chapter 1 — Setting Up Your Environment →](chapter-01.md)
