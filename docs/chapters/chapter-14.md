# Chapter 14 — File Handling

## Introduction

Every program so far has read from the keyboard and written to the screen — `stdin` and `stdout`, introduced in Chapter 5. Those are useful, but data typed at a keyboard disappears the moment the program ends, and data printed to a screen cannot easily be reused by another program. This chapter extends the same I/O ideas to **files** on disk: data that persists after the program exits, and that other programs (or later runs of the same program) can read back.

## `FILE *` and Streams

Recall from Chapter 5 that `stdin`, `stdout`, and `stderr` are streams — abstractions for a flow of data, without needing to know the exact underlying details. All three are represented in C by the type `FILE *`, declared in `<stdio.h>`. Working with a file on disk uses exactly the same type and largely the same functions: instead of being handed `stdin` or `stdout` automatically, you obtain a `FILE *` for a specific file by opening it yourself, and you must close it explicitly when finished.

```c
FILE *fopen(const char *filename, const char *mode);
```

`fopen` opens the file named `filename` according to `mode`, and returns a pointer to a `FILE` object you use for all further operations on that file, or `NULL` if the file could not be opened.

## `fopen` Modes

| Mode | Meaning |
|---|---|
| `"r"` | Open an existing file for reading. Fails (`NULL`) if the file does not exist. |
| `"w"` | Open for writing. Creates the file if it doesn't exist; **truncates** (erases) the existing contents if it does. |
| `"a"` | Open for appending. Creates the file if it doesn't exist; writes go to the end of existing content, never overwriting it. |
| `"r+"` | Open an existing file for both reading and writing, starting at the beginning. Fails if the file does not exist. |
| `"w+"` | Open for both reading and writing; truncates the file if it exists, or creates it if not. |
| `"a+"` | Open for both reading and appending; creates the file if it doesn't exist; writes always go to the end. |
| `"b"` suffix | Append `b` to any mode above (e.g., `"rb"`, `"wb"`) to open in **binary mode** rather than text mode — relevant for `fread`/`fwrite`, covered later in this chapter. |

!!! warning "`\"w\"` erases existing content"
    Opening a file with `"w"` immediately and silently discards everything already in that file, the moment `fopen` succeeds — before you write a single byte. If you intend to keep existing content, use `"a"` (append) or `"r+"` instead.

### Always Check `fopen`'s Return Value

A file might not exist, the path might be wrong, or the program might lack permission to read or write it. `fopen` reports any such failure by returning `NULL` rather than crashing — and just as with `malloc` in Chapter 13, a program that proceeds to use a `NULL` `FILE *` invokes undefined behavior. Always check.

```c
#include <stdio.h>

int main(void) {
    FILE *fp = fopen("does_not_exist.txt", "r");

    if (fp == NULL) {
        perror("fopen failed");
        return 1;
    }

    fclose(fp);
    return 0;
}
```

### Compile and run

```bash
gcc -std=c11 -Wall -Wextra checkopen.c -o checkopen
./checkopen
```

### Expected output

```text
fopen failed: No such file or directory
```

`perror` (declared in `<stdio.h>`) prints the string you give it, followed by a colon, a space, and a human-readable description of the most recent system error — far more informative than a generic message you write by hand. An equivalent manual alternative, useful when you want full control over the wording, is:

```c
if (fp == NULL) {
    fprintf(stderr, "Error: could not open file.\n");
    return 1;
}
```

## Text I/O: Writing and Reading Lines

### `fprintf` and `fputs`

`fprintf` works exactly like `printf`, except it takes an extra first argument specifying which stream to write to — meaning `printf("...")` is, in fact, just shorthand for `fprintf(stdout, "...")`.

```c
#include <stdio.h>

int main(void) {
    FILE *fp = fopen("greeting.txt", "w");

    if (fp == NULL) {
        perror("fopen failed");
        return 1;
    }

    fprintf(fp, "Hello, file!\n");
    fputs("This is line two.\n", fp);

    fclose(fp);

    printf("Wrote greeting.txt successfully.\n");
    return 0;
}
```

### Compile and run

```bash
gcc -std=c11 -Wall -Wextra writefile.c -o writefile
./writefile
cat greeting.txt
```

### Expected output

```text
Wrote greeting.txt successfully.
Hello, file!
This is line two.
```

### Reading a File Line by Line with `fgets`

The standard, safe pattern for reading a text file one line at a time uses `fgets` in a `while` loop, recalling its safety properties from Chapter 5: it takes a maximum size and never writes past the end of the buffer.

```c
#include <stdio.h>

int main(void) {
    FILE *fp = fopen("greeting.txt", "r");

    if (fp == NULL) {
        perror("fopen failed");
        return 1;
    }

    char line[100];
    int line_number = 1;

    while (fgets(line, sizeof(line), fp) != NULL) {
        printf("%d: %s", line_number, line);
        line_number++;
    }

    fclose(fp);
    return 0;
}
```

### Compile and run

```bash
gcc -std=c11 -Wall -Wextra readfile.c -o readfile
./readfile
```

### Expected output

```text
1: Hello, file!
2: This is line two.
```

`fgets` returns `NULL` once it reaches the end of the file (or on an error), which is exactly the condition the `while` loop checks to know when to stop. Note that `line` already includes the trailing `\n` from the file (if present), which is why no extra newline is added in the `printf` call.

### `fscanf` for Formatted Reading

`fscanf` is `scanf`'s counterpart for files, reading formatted values rather than whole lines — and it inherits the same pitfalls discussed in Chapter 5 (leftover newlines, the need to check its return value).

```c
#include <stdio.h>

int main(void) {
    FILE *fp = fopen("numbers.txt", "w");
    if (fp == NULL) {
        perror("fopen failed");
        return 1;
    }
    fprintf(fp, "10 20 30\n");
    fclose(fp);

    fp = fopen("numbers.txt", "r");
    if (fp == NULL) {
        perror("fopen failed");
        return 1;
    }

    int a, b, c;
    int items_read = fscanf(fp, "%d %d %d", &a, &b, &c);

    if (items_read == 3) {
        printf("Sum: %d\n", a + b + c);
    } else {
        fprintf(stderr, "Expected 3 numbers, got %d\n", items_read);
    }

    fclose(fp);
    return 0;
}
```

### Compile and run

```bash
gcc -std=c11 -Wall -Wextra fscanfdemo.c -o fscanfdemo
./fscanfdemo
```

### Expected output

```text
Sum: 60
```

## Binary I/O: `fread` and `fwrite`

Text mode formats every value as human-readable characters — an `int` like `1000000` becomes the seven characters `'1'`, `'0'`, `'0'`, `'0'`, `'0'`, `'0'`, `'0'`. **Binary mode** instead writes the exact bytes of a value's in-memory representation, with no conversion to text at all.

```c
size_t fwrite(const void *ptr, size_t size, size_t count, FILE *stream);
size_t fread(void *ptr, size_t size, size_t count, FILE *stream);
```

Both take a pointer to the data, the size of one element, how many elements, and the stream — and both return the number of elements actually written or read, which you should check against the number you requested.

```c
#include <stdio.h>

typedef struct {
    char name[20];
    int score;
} Record;

int main(void) {
    Record records[3] = {
        {"Asha", 88},
        {"Brijesh", 75},
        {"Chitra", 91}
    };

    FILE *fp = fopen("records.bin", "wb");
    if (fp == NULL) {
        perror("fopen failed");
        return 1;
    }

    size_t written = fwrite(records, sizeof(Record), 3, fp);
    printf("Wrote %zu records.\n", written);

    fclose(fp);

    Record loaded[3];
    fp = fopen("records.bin", "rb");
    if (fp == NULL) {
        perror("fopen failed");
        return 1;
    }

    size_t read_count = fread(loaded, sizeof(Record), 3, fp);
    printf("Read %zu records.\n", read_count);

    for (size_t i = 0; i < read_count; i++) {
        printf("%s: %d\n", loaded[i].name, loaded[i].score);
    }

    fclose(fp);
    return 0;
}
```

### Compile and run

```bash
gcc -std=c11 -Wall -Wextra binaryio.c -o binaryio
./binaryio
```

### Expected output

```text
Wrote 3 records.
Read 3 records.
Asha: 88
Brijesh: 75
Chitra: 91
```

The `"b"` suffix in `"wb"` and `"rb"` requests binary mode. On many systems text and binary mode behave identically for plain ASCII text, but on some platforms (notably Windows) text mode silently translates line-ending characters, which would corrupt non-text data — so binary mode is the correct, portable choice whenever you use `fread`/`fwrite`.

!!! note "Binary files are compact and exact, but not portable"
    Binary I/O writes the struct's raw in-memory bytes directly, with no formatting overhead — faster and more compact than text. The tradeoff is that the file is not human-readable, and it is **not guaranteed portable** between different machines: the compiler may insert different padding bytes between struct members (recall the padding note in Chapter 12) on different platforms, and different machines may store multi-byte values in different byte orders (endianness). A binary file written by one program is only guaranteed to be read correctly by a program compiled for a compatible platform with a compatible struct layout.

## `fclose`: Closing What You Open

`fclose(fp)` closes a file, and you should call it on every file you open, for two reasons. First, the operating system allows only a limited number of files to be open at once per program; failing to close files can exhaust that limit. Second, and more subtly, writes are often **buffered** — held temporarily in memory rather than sent to disk immediately, for efficiency. `fclose` flushes any buffered writes to disk before closing; a program that exits, or crashes, without closing a file risks losing data that was written but never actually flushed to disk.

`fclose` returns an `int` (0 on success) which can be checked, though in simple programs the return value is often not checked separately from the operations that preceded it.

## File Positioning: `fseek`, `ftell`, `rewind`

Every open file has a **position indicator** — where the next read or write will occur. Reading and writing normally advance it automatically, but you can also move it explicitly.

```c
int fseek(FILE *stream, long offset, int whence);
long ftell(FILE *stream);
void rewind(FILE *stream);
```

`fseek` moves the position indicator by `offset` bytes, relative to `whence`, which is one of `SEEK_SET` (from the beginning of the file), `SEEK_CUR` (from the current position), or `SEEK_END` (from the end of the file). `ftell` reports the current position as a byte offset from the start. `rewind(fp)` is a convenience shorthand exactly equivalent to `fseek(fp, 0, SEEK_SET)` — moving back to the very beginning.

```c
#include <stdio.h>

int main(void) {
    FILE *fp = fopen("seekdemo.txt", "w+");
    if (fp == NULL) {
        perror("fopen failed");
        return 1;
    }

    fputs("0123456789", fp);

    long pos = ftell(fp);
    printf("Position after writing: %ld\n", pos);

    rewind(fp);
    printf("Position after rewind: %ld\n", ftell(fp));

    char buffer[6];
    fread(buffer, 1, 5, fp);
    buffer[5] = '\0';
    printf("First 5 characters: %s\n", buffer);

    fseek(fp, 5, SEEK_SET);
    fread(buffer, 1, 5, fp);
    buffer[5] = '\0';
    printf("Characters from offset 5: %s\n", buffer);

    fclose(fp);
    return 0;
}
```

### Compile and run

```bash
gcc -std=c11 -Wall -Wextra seekdemo.c -o seekdemo
./seekdemo
```

### Expected output

```text
Position after writing: 10
Position after rewind: 0
First 5 characters: 01234
Characters from offset 5: 56789
```

## Worked Example: Write, Reopen, and Number Each Line

The following program writes a short list of lines to a text file, then reopens it (a fresh `fopen`, demonstrating that the data really did persist to disk) and prints each line prefixed with its line number.

```c
#include <stdio.h>

int main(void) {
    FILE *fp = fopen("notes.txt", "w");
    if (fp == NULL) {
        perror("fopen failed");
        return 1;
    }

    fputs("Buy groceries\n", fp);
    fputs("Finish chapter 14\n", fp);
    fputs("Call a friend\n", fp);
    fclose(fp);

    fp = fopen("notes.txt", "r");
    if (fp == NULL) {
        perror("fopen failed");
        return 1;
    }

    char line[100];
    int number = 1;
    while (fgets(line, sizeof(line), fp) != NULL) {
        printf("%d. %s", number, line);
        number++;
    }

    fclose(fp);
    return 0;
}
```

### Compile and run

```bash
gcc -std=c11 -Wall -Wextra notes.c -o notes
./notes
```

### Expected output

```text
1. Buy groceries
2. Finish chapter 14
3. Call a friend
```

## Worked Example: Appending a Log Entry Each Run

A common real use of `"a"` mode is a simple log file: each run of the program adds one new entry, without disturbing any entries written by previous runs.

```c
#include <stdio.h>

int main(void) {
    FILE *fp = fopen("run_log.txt", "a");

    if (fp == NULL) {
        perror("fopen failed");
        return 1;
    }

    fputs("Program executed.\n", fp);

    fclose(fp);

    printf("Logged this run to run_log.txt\n");
    return 0;
}
```

### Compile and run

```bash
gcc -std=c11 -Wall -Wextra logappend.c -o logappend
./logappend
./logappend
./logappend
cat run_log.txt
```

### Expected output

```text
Logged this run to run_log.txt
Logged this run to run_log.txt
Logged this run to run_log.txt
Program executed.
Program executed.
Program executed.
```

Each run prints its own confirmation to the terminal; the final `cat` shows that `run_log.txt` accumulated one `"Program executed."` line per run, since `"a"` mode always writes at the end without truncating prior content — in contrast to `"w"` mode, which would have erased the file on each run, leaving only the most recent entry.

## Summary

- `FILE *`, from `<stdio.h>`, represents a stream — the same abstraction behind `stdin`/`stdout`/`stderr` (Chapter 5) — for a file on disk; `fopen` obtains one, and `fclose` releases it.
- `fopen` modes: `"r"` read (file must exist), `"w"` write (truncates or creates), `"a"` append (creates or adds to the end), and the `+` variants for combined reading and writing; append `b` for binary mode.
- Always check `fopen`'s return for `NULL`, using `perror` or a manual message to report the failure.
- `fprintf`/`fputs` write text; the standard line-reading loop is `while (fgets(line, sizeof(line), fp) != NULL) { ... }`; `fscanf` reads formatted values with the same pitfalls as `scanf`.
- `fread`/`fwrite` transfer raw bytes in binary mode — compact and exact, but not human-readable and not portable across machines with different struct padding or endianness.
- `fclose` must be called on every opened file, both to free the limited number of file handles and to flush buffered writes to disk.
- `fseek`, `ftell`, and `rewind` move and query the file's position indicator, enabling random access rather than strictly sequential reading.

## Exercises

1. Write a program that opens a file with `"w"`, writes five lines of your choosing, closes it, then reopens it with `"r"` and prints the total number of lines found.
2. Write a program that reads a filename from the user (recall `fgets`/`scanf` input techniques from Chapter 5) and attempts to open it for reading; if `fopen` fails, print a clear error using `perror` and exit with a non-zero status instead of continuing.
3. Using the `Record` struct from this chapter's binary I/O example, write a program that writes an array of 5 records to a binary file, then writes a second, separate program (or a second `main`-callable function) that reads them back and prints only the records with a score above 80.
4. Extend the appending-log worked example so that each entry includes a sequential entry number read back from the file (hint: count the existing lines first by reading the file in `"r"` mode before reopening it in `"a"` mode to append the next-numbered entry).
5. *(Harder)* Write a program that copies one text file to another, line by line, converting every lowercase letter to uppercase as it goes (recall character functions or manual ASCII arithmetic from Chapter 10). Test it on a file with several lines of mixed-case text.
6. *(Harder)* Write a program that opens a binary file containing a sequence of `int` values (write a small helper section first, or a separate setup program, to create one), uses `fseek` with `SEEK_END` and `ftell` to determine how many integers it contains without reading them all sequentially, then uses `fseek` with `SEEK_SET` to jump directly to and print only the last 3 integers in the file.

---
[← Chapter 13 — Dynamic Memory Allocation](chapter-13.md) | [Chapter 15 — Preprocessor Directives and Macros →](chapter-15.md)
