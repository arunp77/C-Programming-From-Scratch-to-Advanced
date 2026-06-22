# Appendix A — C Standard Library Reference

This appendix collects the standard library functions referenced across this tutorial, organized by header, along with a handful of other commonly used functions from the same headers so this page is useful as a general reference, not just a recap.

## `<stdio.h>`

| Function | Signature | Description |
|---|---|---|
| `printf` | `int printf(const char *format, ...)` | Writes formatted output to `stdout`. Returns the number of characters written, or a negative value on error. |
| `fprintf` | `int fprintf(FILE *stream, const char *format, ...)` | Writes formatted output to the given stream (e.g. `stderr` for error messages). |
| `sprintf` | `int sprintf(char *buffer, const char *format, ...)` | Writes formatted output into a string buffer instead of a stream. Caller must ensure the buffer is large enough. |
| `snprintf` | `int snprintf(char *buffer, size_t n, const char *format, ...)` | Like `sprintf`, but never writes more than `n - 1` characters plus a null terminator — the safe, bounds-checked alternative. |
| `scanf` | `int scanf(const char *format, ...)` | Reads formatted input from `stdin`. Returns the number of items successfully matched and assigned. |
| `fscanf` | `int fscanf(FILE *stream, const char *format, ...)` | Like `scanf`, but reads from the given stream instead of `stdin`. |
| `sscanf` | `int sscanf(const char *str, const char *format, ...)` | Reads formatted input from a string instead of a stream. |
| `fgets` | `char *fgets(char *str, int n, FILE *stream)` | Reads at most `n - 1` characters (or up to a newline) from a stream into `str`, null-terminating the result. The safe alternative to `gets`. |
| `fputs` | `int fputs(const char *str, FILE *stream)` | Writes a string to a stream, without appending a newline. |
| `fopen` | `FILE *fopen(const char *filename, const char *mode)` | Opens a file and returns a stream pointer, or `NULL` on failure. Modes include `"r"`, `"w"`, `"a"`, `"rb"`, `"wb"`, etc. |
| `fclose` | `int fclose(FILE *stream)` | Closes an open stream, flushing any buffered output. Returns `0` on success. |
| `fread` | `size_t fread(void *ptr, size_t size, size_t nmemb, FILE *stream)` | Reads `nmemb` elements of `size` bytes each from a stream into `ptr`. Returns the number of elements actually read. |
| `fwrite` | `size_t fwrite(const void *ptr, size_t size, size_t nmemb, FILE *stream)` | Writes `nmemb` elements of `size` bytes each from `ptr` to a stream. Returns the number of elements actually written. |
| `fseek` | `int fseek(FILE *stream, long offset, int whence)` | Repositions a stream's file pointer. `whence` is one of `SEEK_SET`, `SEEK_CUR`, `SEEK_END`. |
| `ftell` | `long ftell(FILE *stream)` | Returns the current position of a stream's file pointer, or `-1L` on error. |
| `rewind` | `void rewind(FILE *stream)` | Resets a stream's file pointer to the beginning of the file. |
| `feof` | `int feof(FILE *stream)` | Returns non-zero if the end-of-file indicator is set for the stream. |
| `perror` | `void perror(const char *str)` | Prints `str` followed by a description of the most recent system error, to `stderr`. |

## `<stdlib.h>`

| Function | Signature | Description |
|---|---|---|
| `malloc` | `void *malloc(size_t size)` | Allocates `size` bytes of uninitialized heap memory; returns `NULL` on failure. |
| `calloc` | `void *calloc(size_t nmemb, size_t size)` | Allocates space for `nmemb` elements of `size` bytes each, zero-initialized; returns `NULL` on failure. |
| `realloc` | `void *realloc(void *ptr, size_t size)` | Resizes a previously allocated block to `size` bytes, possibly moving it; returns `NULL` on failure (original block remains valid). |
| `free` | `void free(void *ptr)` | Releases a block of memory previously returned by `malloc`/`calloc`/`realloc`. `free(NULL)` is explicitly safe and does nothing. |
| `exit` | `void exit(int status)` | Terminates the program immediately, flushing streams and running registered cleanup handlers. `status` of `0` (or `EXIT_SUCCESS`) conventionally signals success. |
| `atoi` | `int atoi(const char *str)` | Converts a string to an `int`. No error reporting for malformed input — returns `0` if no valid conversion is found. |
| `atof` | `double atof(const char *str)` | Converts a string to a `double`. Like `atoi`, offers no error reporting. |
| `atol` | `long atol(const char *str)` | Converts a string to a `long`. |
| `strtol` | `long strtol(const char *str, char **endptr, int base)` | Converts a string to a `long`, with error detection via `endptr` and support for a specified numeric base. The robust alternative to `atoi`. |
| `qsort` | `void qsort(void *base, size_t nmemb, size_t size, int (*compar)(const void *, const void *))` | Sorts an array of `nmemb` elements of `size` bytes each in place, using a caller-supplied comparator function pointer. |
| `rand` | `int rand(void)` | Returns a pseudo-random integer between `0` and `RAND_MAX`. |
| `srand` | `void srand(unsigned int seed)` | Seeds the pseudo-random number generator used by `rand`. Commonly seeded with `time(NULL)` for different sequences on each run. |
| `abs` | `int abs(int n)` | Returns the absolute value of an `int`. |

## `<string.h>`

| Function | Signature | Description |
|---|---|---|
| `strlen` | `size_t strlen(const char *str)` | Returns the length of a null-terminated string, not counting the terminator. |
| `strcpy` | `char *strcpy(char *dest, const char *src)` | Copies `src` into `dest`, including the null terminator. Performs no bounds checking — `dest` must be large enough. |
| `strncpy` | `char *strncpy(char *dest, const char *src, size_t n)` | Copies at most `n` characters from `src` into `dest`. Does not guarantee null-termination if `src` is `n` characters or longer. |
| `strcat` | `char *strcat(char *dest, const char *src)` | Appends `src` to the end of `dest`. Performs no bounds checking. |
| `strncat` | `char *strncat(char *dest, const char *src, size_t n)` | Appends at most `n` characters from `src` to `dest`, always null-terminating the result. |
| `strcmp` | `int strcmp(const char *a, const char *b)` | Compares two strings lexicographically. Returns `0` if equal, negative if `a < b`, positive if `a > b`. |
| `strncmp` | `int strncmp(const char *a, const char *b, size_t n)` | Like `strcmp`, but compares at most `n` characters. |
| `strchr` | `char *strchr(const char *str, int c)` | Returns a pointer to the first occurrence of character `c` in `str`, or `NULL` if not found. |
| `strstr` | `char *strstr(const char *haystack, const char *needle)` | Returns a pointer to the first occurrence of the substring `needle` within `haystack`, or `NULL` if not found. |
| `strtok` | `char *strtok(char *str, const char *delim)` | Splits a string into tokens separated by characters in `delim`. Modifies the input string and retains state between calls — not thread-safe. |
| `memcpy` | `void *memcpy(void *dest, const void *src, size_t n)` | Copies `n` bytes from `src` to `dest`. Source and destination must not overlap. |
| `memmove` | `void *memmove(void *dest, const void *src, size_t n)` | Like `memcpy`, but safe even when source and destination overlap. |
| `memset` | `void *memset(void *ptr, int value, size_t n)` | Sets the first `n` bytes of `ptr` to `value` (interpreted as `unsigned char`). |
| `memcmp` | `int memcmp(const void *a, const void *b, size_t n)` | Compares the first `n` bytes of two memory blocks, returning negative/zero/positive like `strcmp`. |

## `<math.h>`

| Function | Signature | Description |
|---|---|---|
| `sqrt` | `double sqrt(double x)` | Returns the square root of `x`. Domain error for negative `x`. |
| `pow` | `double pow(double base, double exponent)` | Returns `base` raised to `exponent`. |
| `fabs` | `double fabs(double x)` | Returns the absolute value of a floating-point number. (Use `abs` from `<stdlib.h>` for plain `int` values.) |
| `floor` | `double floor(double x)` | Rounds `x` down to the nearest integer value, as a `double`. |
| `ceil` | `double ceil(double x)` | Rounds `x` up to the nearest integer value, as a `double`. |
| `round` | `double round(double x)` | Rounds `x` to the nearest integer value, halfway cases rounded away from zero. |
| `fmod` | `double fmod(double x, double y)` | Returns the floating-point remainder of `x / y`. |
| `exp` | `double exp(double x)` | Returns *e* raised to the power `x`. |
| `log` | `double log(double x)` | Returns the natural logarithm (base *e*) of `x`. |
| `log10` | `double log10(double x)` | Returns the base-10 logarithm of `x`. |
| `sin`, `cos`, `tan` | `double sin(double x)` etc. | Standard trigonometric functions, arguments in radians. |

!!! note "Linking math.h on some systems"
    On some Linux toolchains, code using `<math.h>` functions must be linked with `-lm` (e.g. `gcc program.c -o program -lm`), since the math library is not always linked in by default.

## `<ctype.h>`

| Function | Signature | Description |
|---|---|---|
| `isdigit` | `int isdigit(int c)` | Returns non-zero if `c` is a decimal digit (`0`–`9`). |
| `isalpha` | `int isalpha(int c)` | Returns non-zero if `c` is an alphabetic letter. |
| `isalnum` | `int isalnum(int c)` | Returns non-zero if `c` is alphabetic or a digit. |
| `isspace` | `int isspace(int c)` | Returns non-zero if `c` is whitespace (space, tab, newline, etc.). |
| `isupper` | `int isupper(int c)` | Returns non-zero if `c` is an uppercase letter. |
| `islower` | `int islower(int c)` | Returns non-zero if `c` is a lowercase letter. |
| `ispunct` | `int ispunct(int c)` | Returns non-zero if `c` is a punctuation character. |
| `toupper` | `int toupper(int c)` | Returns the uppercase equivalent of `c`, or `c` unchanged if it has none. |
| `tolower` | `int tolower(int c)` | Returns the lowercase equivalent of `c`, or `c` unchanged if it has none. |

## `<time.h>`

| Function | Signature | Description |
|---|---|---|
| `time` | `time_t time(time_t *timer)` | Returns the current calendar time as a `time_t`; if `timer` is non-`NULL`, also stores the result there. Commonly used to seed `srand`. |
| `difftime` | `double difftime(time_t end, time_t start)` | Returns the difference, in seconds, between two `time_t` values. |
| `clock` | `clock_t clock(void)` | Returns an approximation of processor time used by the program, in `CLOCKS_PER_SEC` units — useful for rough performance timing. |
| `localtime` | `struct tm *localtime(const time_t *timer)` | Converts a `time_t` into a broken-down local-time `struct tm` (year, month, day, hour, etc.). |
| `strftime` | `size_t strftime(char *str, size_t n, const char *format, const struct tm *tm)` | Formats a `struct tm` into a human-readable string according to a format specification, similar in spirit to `printf`. |

---
[← Chapter 20 — Debugging and Best Practices](chapter-20.md) | [Appendix B — Cheat Sheets →](Appendix-B.md)
