# Chapter 1 — Setting Up Your Environment

## Introduction

Before we can write any C code, we need to install some tools on your computer. In this chapter we will install a **compiler** (the program that turns your C source code into a runnable program), a **debugger** (a tool for inspecting a program while it runs, to help find bugs), and a comfortable place to write code — a **text editor**. We will also cover the absolute basics of using a **terminal**, since almost everything in this tutorial happens there.

If you have never installed developer tools or used a terminal before, take this chapter slowly. Every later chapter assumes the setup described here is working.

## What Is a Compiler, Again?

In Chapter 0 we said that a compiler translates human-readable source code into machine code that the CPU can execute directly, producing a standalone executable file. Let's make that concrete with the specific tool we'll use: **gcc**, the GNU Compiler Collection.

When you write a C program in a plain text file (by convention named with a `.c` extension, like `hello.c`), that file is just text — the computer cannot run it as-is. You hand that file to `gcc`, and `gcc` performs several internal steps (which we will dissect in detail in Chapter 2) to produce an executable file. You then run *that* executable. gcc itself is not involved when your program actually runs — its job is done once the executable exists.

This is worth contrasting with an interpreter one more time: if C were interpreted, you would hand your `.c` file to an interpreter program every single time you wanted to run it, and the interpreter would translate-and-execute on the fly, line by line, with no separate reusable executable produced. gcc does not work this way — it does its translation work once, up front.

!!! note "gcc vs. 'a C compiler'"
    gcc is *a* compiler for C (and several other languages), not the only one. Another extremely common one is **clang**. Everything in this tutorial works the same way conceptually with clang; we standardize on gcc because it is the most universally available option across Linux, macOS, and Windows (via WSL or MSYS2).

## Installing a C Toolchain

A **toolchain** is the set of programs needed to go from source code to a running executable: the compiler itself, plus supporting tools like a linker (covered in Chapter 2) and a debugger. Follow the instructions for your operating system.

### Ubuntu / Debian / WSL (Windows Subsystem for Linux)

Open a terminal (we'll explain what that is and how to use it shortly) and run:

```bash
sudo apt update && sudo apt install build-essential gdb
```

`build-essential` is a bundle package that includes `gcc`, a linker, and other essentials. `gdb` is the **GNU Debugger**, a tool that lets you pause a running program and inspect what it's doing — useful from Chapter 2 onward when we look at compiler errors, and essential later when tracking down trickier bugs.

### Fedora

```bash
sudo dnf groupinstall "Development Tools"
sudo dnf install gdb
```

### macOS

Apple bundles a C compiler (clang, which also responds to the `gcc` command as an alias on macOS) with its **Command Line Tools** package. Install it with:

```bash
xcode-select --install
```

This opens a small installer dialog; follow the prompts. You do not need to install the full Xcode application for this tutorial — the Command Line Tools are enough.

### Windows

On Windows you have two good options. Read both descriptions and pick the one that suits you; either is a completely valid way to follow this tutorial.

**Option A: WSL (recommended for beginners).** WSL, the Windows Subsystem for Linux, lets you run a real Linux environment directly inside Windows. This means you can follow the exact same Ubuntu/Debian instructions above, inside that Linux environment, and everything in this tutorial — including terminal commands — will behave identically to a Linux machine. To install it, open PowerShell as Administrator and run:

```text
wsl --install
```

Restart when prompted, then open the "Ubuntu" application from your Start menu. You'll be asked to create a Linux username and password (these are separate from your Windows login). Once you have a terminal prompt, follow the Ubuntu/Debian instructions above (`sudo apt update && sudo apt install build-essential gdb`).

**Option B: MSYS2.** MSYS2 is a tool that provides a Unix-like terminal environment and package manager directly on native Windows, without a separate Linux installation. Download and run the installer from the MSYS2 project's website, then, inside the MSYS2 terminal it opens, run:

```bash
pacman -Syu
pacman -S mingw-w64-ucrt-x86_64-gcc mingw-w64-ucrt-x86_64-gdb
```

The first command updates MSYS2's own package database (you may need to close and reopen the terminal and re-run it once, as it instructs). The second installs gcc and gdb built for native Windows.

!!! tip "Which should I choose on Windows?"
    If you're not sure, choose WSL. It is the closest to what the majority of C tutorials, Stack Overflow answers, and real-world Linux servers look like, so the skills transfer most directly. MSYS2 is a fine choice if you specifically want a compiler that produces native Windows `.exe` files without any Linux layer involved.

## Installing and Configuring a Text Editor

You need somewhere to write your C source files. We recommend **Visual Studio Code** (VS Code), a free, widely used code editor, but any plain text editor plus a terminal works fine — there is nothing magic about a particular editor, and experienced programmers sometimes deliberately use very simple ones.

### Installing VS Code

Download VS Code from its official website for your operating system and run the installer, accepting the defaults. If you're using WSL on Windows, install VS Code on the **Windows** side as normal; it has a WSL integration feature that lets it edit files inside your Linux environment seamlessly.

### Adding the C/C++ Extension

Inside VS Code:

1. Click the Extensions icon in the left-hand sidebar (it looks like four small squares).
2. Search for "C/C++" (the extension published by Microsoft).
3. Click **Install**.

This extension gives you syntax highlighting (color-coding of your code) and some basic error-checking as you type. It does not replace gcc — you still compile and run from the terminal as shown below.

!!! tip "Editors are a matter of taste"
    If you already have a preferred plain text editor (even something as simple as Notepad, gedit, or nano) and a terminal, you can absolutely use that instead of VS Code for this entire tutorial. The only requirement is that the editor saves plain text files without adding any hidden formatting — word processors like Microsoft Word are not suitable, because they save rich text/formatting data, not plain source code.

## Terminal Basics for Absolute Beginners

A **terminal** (also called a "command line" or "shell") is a text-based way of interacting with your computer: instead of clicking icons, you type commands and press Enter. This will feel unfamiliar at first if you've only ever used a mouse-and-icons interface, but it is the primary tool of this entire tutorial, so let's cover the essentials now.

When you open a terminal, you'll see a **prompt** — some text (often ending in `$` or `%`) followed by a blinking cursor, waiting for you to type a command.

**`pwd`** — "print working directory." Shows you which folder (directory) you are currently "in."

```bash
pwd
```

```text
/home/yourname
```

**`ls`** — "list." Shows the files and folders inside your current directory. (On Windows, outside of WSL/MSYS2, the equivalent built-in command is `dir`, but inside WSL or MSYS2 you use `ls` exactly as on Linux/macOS.)

```bash
ls
```

```text
Desktop  Documents  Downloads
```

**`cd`** — "change directory." Moves you into a different folder.

```bash
cd Documents
```

After this, `pwd` would show `/home/yourname/Documents`. To go back up one level, use `cd ..` (two dots mean "the parent folder"). To jump straight to your home folder from anywhere, just type `cd` with nothing after it.

**`mkdir`** — "make directory." Creates a new folder.

```bash
mkdir c_practice
```

This creates a folder named `c_practice` inside your current directory.

**Creating a file.** You can create an empty file from the terminal using `touch` (Linux/macOS/WSL):

```bash
touch hello.c
```

But in practice, you will usually create your `.c` files directly from your text editor by choosing "New File" and saving it with a `.c` extension — that's exactly what we'll do next.

!!! note "A typical beginner workflow"
    A common pattern you'll repeat constantly in this tutorial: `cd` into the folder where you keep your practice code, open or create a `.c` file in your editor, write some code, save it, then switch to the terminal and compile/run it. We'll do this together right now.

## Verifying Your Installation

First, confirm gcc is installed and check its version:

```bash
gcc --version
```

You should see output similar to this (the exact version number will vary):

```text
gcc (Ubuntu 11.4.0-1ubuntu1~22.04) 11.4.0
Copyright (C) 2021 Free Software Foundation, Inc.
This is free software; see the source for copying conditions.
```

If you see something like this, gcc is correctly installed. If instead you see an error, skip to the Troubleshooting section below.

Now let's write the smallest possible test program and compile it, just to confirm the whole pipeline — editor, compiler, and terminal — works together. Create a folder and file:

```bash
mkdir -p ~/c_practice/ch01
cd ~/c_practice/ch01
```

Using your text editor, create a file named `test.c` in that folder with exactly this content. (We are not explaining this code yet — that is the entire subject of Chapter 2. For now, just type it exactly as shown.)

```c
#include <stdio.h>

int main(void) {
    printf("Setup works!\n");
    return 0;
}
```

Save the file, then in your terminal (making sure you're still in the `~/c_practice/ch01` folder), compile it:

```bash
gcc test.c -o test
```

If nothing prints out and you're returned to the prompt, the compilation succeeded — no output is gcc's way of saying "everything was fine." Now run the program it produced:

```bash
./test
```

```text
Setup works!
```

If you see `Setup works!` printed, congratulations — your entire toolchain (editor, compiler, terminal) is working correctly, and you are ready for Chapter 2.

!!! note "Why `./test` and not just `test`?"
    The leading `./` tells the terminal "look for an executable named `test` in the current directory." Without it, on Linux/macOS, the terminal would search only in a predefined list of system folders for safety reasons, and likely not find your freshly built program. On native Windows (MSYS2), you'd run it as `test.exe` or `./test.exe` instead.

## Troubleshooting: "gcc: command not found"

If running `gcc --version` gives you an error like `gcc: command not found` (Linux/macOS/WSL) or `'gcc' is not recognized as an internal or external command` (Windows), the most likely cause is one of the following.

**The installation did not actually complete.** Re-run the install command for your OS from the beginning of this chapter and watch for error messages partway through. A slow or interrupted network connection is a common cause on shared or constrained machines.

**The terminal session is "stale."** If you had a terminal window open *before* you installed the toolchain, close that window completely and open a brand-new terminal. Many installers update settings that only take effect in new terminal sessions.

**PATH issues.** Your operating system finds commands like `gcc` by searching a list of folders called the **PATH**. If gcc was installed into a folder that isn't on your PATH, the terminal won't find it even though the program exists on disk.

- *Linux/WSL*: this is rare with `apt`/`dnf` installs, since they install into standard system locations already on the PATH. If it does happen, confirm the package actually installed with `dpkg -l | grep build-essential` (Debian/Ubuntu) and reinstall if it's missing.
- *macOS*: make sure the Command Line Tools install finished (`xcode-select --install` shows a completed dialog, not still "installing"). You can check the active path with `xcode-select -p`.
- *MSYS2*: make sure you are opening the correct shortcut — MSYS2 installs several similarly named terminal shortcuts (e.g. "MSYS2 UCRT64"); you must use the one matching the package set you installed (`mingw-w64-ucrt-x86_64-gcc` corresponds to the "MSYS2 UCRT64" shortcut). Opening the wrong one will not see the gcc you installed.

If you are stuck after trying the above, it is genuinely fine to close every terminal/editor window, restart your computer, and try the version check again — this resolves a surprising number of PATH problems by forcing all programs to reload their environment.

## Summary

- A compiler (gcc, in this tutorial) translates C source code into a standalone executable, ahead of running it; this differs from an interpreter, which translates and runs code on the fly.
- Install a C toolchain appropriate to your OS: `build-essential` + `gdb` on Ubuntu/Debian/WSL, the "Development Tools" group + `gdb` on Fedora, Command Line Tools on macOS, and either WSL or MSYS2 on Windows.
- VS Code with the C/C++ extension is a good free editor choice, but any plain text editor plus a terminal works.
- Core terminal commands: `pwd` (where am I), `ls`/`dir` (what's here), `cd` (move between folders), `mkdir` (create a folder).
- Verify your setup with `gcc --version`, then write, compile (`gcc file.c -o file`), and run (`./file`) a one-line test program.
- "gcc: command not found" is almost always an incomplete install, a stale terminal session, or a PATH problem — reinstalling, opening a fresh terminal, or restarting the machine resolves most cases.

## Exercises

1. Run `gcc --version` on your machine and write down the exact version string it prints.
2. Using `mkdir` and `cd`, create a folder structure `c_practice/ch01_exercises` and navigate into it.
3. Create a file called `info.c` in that folder containing only a `printf` line of your choosing (you can copy the structure of `test.c` from this chapter), compile it, and run it.
4. Use `ls` to list the contents of your `ch01_exercises` folder both before and after compiling `info.c`. What new file appeared after compiling? Why?
5. *(Harder)* Intentionally rename your test program's source file extension from `.c` to `.txt`, then try compiling it with `gcc info.txt -o info`. Does it still work? What does this tell you about whether the `.c` extension is required by the compiler itself, versus being a convention?
6. *(Harder)* If you have access to more than one of the operating systems described in this chapter (for example, both WSL and native Windows, or a Mac and a Linux machine), install the toolchain on a second one and confirm `gcc --version` works there too, comparing the version numbers.

---
[← Chapter 0 — Introduction to Programming and C](chapter-00.md) | [Chapter 2 — Your First C Program and the Compilation Process →](chapter-02.md)
