# Chapter 10 — Strings

## Introduction

C has no dedicated string type. A **string** in C is simply an array of `char` values, with one special convention: the end of the actual text is marked by a **null byte**, written `'\0'`, a character whose numeric value is `0`. Everything C does with strings — printing them, copying them, measuring their length — relies entirely on this one convention. This chapter covers string literals versus mutable character arrays, the standard `<string.h>` functions, safe versus unsafe ways to read strings, and writing your own basic string-processing code by hand.

## Strings Are `char` Arrays Terminated by `'\0'`

```c
#include <stdio.h>

int main(void) {
    char word[6] = {'H', 'e', 'l', 'l', 'o', '\0'};

    printf("%s\n", word);

    return 0;
}
```

### Compile and run

```bash
gcc -std=c11 -Wall -Wextra manual_string.c -o manual_string
./manual_string
```

### Expected output

```text
Hello
```

`word` is an array of 6 `char`s (recall arrays from Chapter 9): the 5 letters of `"Hello"`, plus a final `'\0'` marking where the actual text ends. `printf`'s `%s` specifier prints characters starting from the array's first element and continues until it encounters that `'\0'` — it does not know or care how big the array's declared capacity actually is; it only looks for the terminator. If the terminator were missing entirely, `%s` would keep reading past the end of the array into whatever memory happens to follow, which is undefined behavior, exactly as with the out-of-bounds array access from Chapter 9.

Writing out every character by hand like this is tedious, so C provides a much more convenient shorthand: a **string literal**.

```c
#include <stdio.h>

int main(void) {
    char word[6] = "Hello"; // equivalent to the character-by-character version above

    printf("%s\n", word);

    return 0;
}
```

### Compile and run

```bash
gcc -std=c11 -Wall -Wextra string_literal_init.c -o string_literal_init
./string_literal_init
```

### Expected output

```text
Hello
```

`"Hello"` as an initializer automatically supplies all 5 letters *and* appends the `'\0'` terminator for you — which is exactly why the array needed to be declared with room for 6 characters, not just 5, even though only 5 letters are visible in the literal.

## String Literals vs. Mutable `char` Arrays vs. `char *`

There are three related but distinct ways strings show up in C code, and confusing them is a common source of bugs:

1. **A string literal**, like `"Hello"` written directly in code, is the text data itself. Depending on the compiler, literals are commonly stored in a read-only region of memory.
2. **A mutable `char` array** initialized from a literal, like `char word[6] = "Hello";`, *copies* the literal's characters into the array's own memory at the start of the program. Because `word` is its own independent array, its contents can be freely modified afterward.
3. **A `char *` pointing directly at a literal**, like `char *word = "Hello";`, does **not** copy anything — `word` simply stores the address of the literal's existing storage. (Full detail on what a pointer actually is, and how `&`/`*` work, is covered in Chapter 11; for strings specifically, the practical consequence below is what matters right now.)

```c
#include <stdio.h>

int main(void) {
    char mutable_word[6] = "Hello";
    mutable_word[0] = 'J'; // fine: this is our own copy of the characters
    printf("Modified array: %s\n", mutable_word);

    char *literal_pointer = "Hello";
    printf("Pointer to literal: %s\n", literal_pointer);
    // literal_pointer[0] = 'J'; // DO NOT do this — see warning below

    return 0;
}
```

### Compile and run

```bash
gcc -std=c11 -Wall -Wextra string_kinds.c -o string_kinds
./string_kinds
```

### Expected output

```text
Modified array: Jello
Pointer to literal: Hello
```

!!! danger "Never modify a string literal through a `char *`"
    The commented-out line, `literal_pointer[0] = 'J';`, is undefined behavior and typically crashes the program (a segmentation fault) on modern systems, because `literal_pointer` points at the literal's original storage, which is commonly placed in a read-only memory region by the compiler. This is different from `mutable_word`, which is a genuine, independent, writable array that merely started out with the same characters. As a rule: if you intend to modify a string's contents, always store it in a `char` array (or otherwise allocate writable memory for it — covered in Chapter 12), never just a `char *` aimed at a literal.

## Common `<string.h>` Functions

`<string.h>` provides the standard library's string-handling functions. All of them rely on the null-terminator convention to know where a string ends.

### `strlen` — String Length

```c
#include <stdio.h>
#include <string.h>

int main(void) {
    char greeting[] = "Hello, world!";

    printf("Length: %zu\n", strlen(greeting));

    return 0;
}
```

### Compile and run

```bash
gcc -std=c11 -Wall -Wextra strlen_demo.c -o strlen_demo
./strlen_demo
```

### Expected output

```text
Length: 13
```

`strlen` counts characters up to (but not including) the terminating `'\0'`, and returns a `size_t` (an unsigned integer type, hence the `%zu` format specifier, also used for `sizeof` in earlier chapters).

### `strcpy` and `strncpy` — Copying

```c
#include <stdio.h>
#include <string.h>

int main(void) {
    char source[] = "Hello";
    char destination[20];

    strcpy(destination, source);
    printf("Copied: %s\n", destination);

    return 0;
}
```

### Compile and run

```bash
gcc -std=c11 -Wall -Wextra strcpy_demo.c -o strcpy_demo
./strcpy_demo
```

### Expected output

```text
Copied: Hello
```

!!! danger "`strcpy` performs no bounds checking"
    `strcpy(destination, source)` keeps copying characters until it reaches `source`'s terminating `'\0'`, with absolutely no awareness of how large `destination` actually is. If `source` is longer than `destination`'s capacity, `strcpy` will write past the end of `destination` — a buffer overflow, exactly as warned about for `scanf("%s", ...)` in Chapter 5 and for raw array indexing in Chapter 9. **`strncpy`** is the bounded alternative: it takes an explicit maximum number of characters to copy.

```c
#include <stdio.h>
#include <string.h>

int main(void) {
    char source[] = "This is a long sentence that won't fit";
    char destination[10];

    strncpy(destination, source, sizeof(destination) - 1);
    destination[sizeof(destination) - 1] = '\0'; // ensure null termination ourselves

    printf("Truncated copy: %s\n", destination);

    return 0;
}
```

### Compile and run

```bash
gcc -std=c11 -Wall -Wextra strncpy_demo.c -o strncpy_demo
./strncpy_demo
```

### Expected output

```text
Truncated copy: This is a
```

!!! warning "`strncpy`'s own gotcha: missing termination"
    `strncpy` copies *at most* the given number of characters, but if `source` is at least that long, it will **not** add a terminating `'\0'` for you — it simply stops after copying exactly that many characters. Forgetting to add the terminator yourself afterward (as the second line above does explicitly: `destination[sizeof(destination) - 1] = '\0';`) leaves `destination` without a proper end marker, and any later use of it with `%s`, `strlen`, or similar will read past its intended content into whatever follows in memory. Always reserve at least one byte of the destination for the terminator (`sizeof(destination) - 1`, as above) and set it explicitly after calling `strncpy`.

### `strcat` and `strncat` — Concatenation

```c
#include <stdio.h>
#include <string.h>

int main(void) {
    char buffer[20] = "Hello, ";

    strcat(buffer, "world!");
    printf("%s\n", buffer);

    return 0;
}
```

### Compile and run

```bash
gcc -std=c11 -Wall -Wextra strcat_demo.c -o strcat_demo
./strcat_demo
```

### Expected output

```text
Hello, world!
```

`strcat(buffer, "world!")` appends the second string onto the end of the first, starting at its existing `'\0'` and overwriting from there — `buffer` must already have enough spare capacity for the combined result plus a new terminator. Exactly like `strcpy`, `strcat` performs no bounds checking, so it carries the same buffer-overflow risk; `strncat` is the bounded equivalent, taking a maximum number of characters to append (and, like `strncpy`, you should still double check the result ends up properly terminated, since the limit applies only to how many characters are *appended*, not to truncating an already-too-long destination).

```c
#include <stdio.h>
#include <string.h>

int main(void) {
    char buffer[15] = "Hello, ";

    strncat(buffer, "wonderful world!", sizeof(buffer) - strlen(buffer) - 1);
    printf("%s\n", buffer);

    return 0;
}
```

### Compile and run

```bash
gcc -std=c11 -Wall -Wextra strncat_demo.c -o strncat_demo
./strncat_demo
```

### Expected output

```text
Hello, wonde
```

The third argument computes exactly how much spare room is left in `buffer` (`sizeof(buffer)` minus the length already used minus one byte reserved for the terminator), so `strncat` cannot write past the end of `buffer` no matter how long the text being appended is.

### `strcmp` and `strncmp` — Comparison

Strings cannot be compared with `==` (that would compare addresses, not contents — explained fully once pointers are covered in Chapter 11). `strcmp` compares string contents and returns `0` if they're equal, a negative number if the first string sorts before the second, and a positive number if it sorts after.

```c
#include <stdio.h>
#include <string.h>

int main(void) {
    char a[] = "apple";
    char b[] = "banana";
    char c[] = "apple";

    printf("strcmp(a, b) = %d\n", strcmp(a, b));
    printf("strcmp(a, c) = %d\n", strcmp(a, c));
    printf("strcmp(b, a) = %d\n", strcmp(b, a));

    return 0;
}
```

### Compile and run

```bash
gcc -std=c11 -Wall -Wextra strcmp_demo.c -o strcmp_demo
./strcmp_demo
```

### Expected output (exact negative/positive magnitudes can vary by implementation)

```text
strcmp(a, b) = -1
strcmp(a, c) = 0
strcmp(b, a) = 1
```

The only guarantee is the *sign*: negative, zero, or positive — never rely on the exact magnitude, only on whether the result is `< 0`, `== 0`, or `> 0`. `strncmp` compares only the first `n` characters, useful when you only care whether two strings share a common prefix.

### `strchr` and `strstr` — Searching

```c
#include <stdio.h>
#include <string.h>

int main(void) {
    char sentence[] = "The quick brown fox";

    char *char_pos = strchr(sentence, 'q');
    char *str_pos = strstr(sentence, "brown");

    if (char_pos != NULL) {
        printf("Found 'q' at position: %ld\n", char_pos - sentence);
    }
    if (str_pos != NULL) {
        printf("Found \"brown\" at position: %ld\n", str_pos - sentence);
    }

    return 0;
}
```

### Compile and run

```bash
gcc -std=c11 -Wall -Wextra strchr_strstr_demo.c -o strchr_strstr_demo
./strchr_strstr_demo
```

### Expected output

```text
Found 'q' at position: 4
Found "brown" at position: 10
```

`strchr` searches for a single character; `strstr` searches for an entire substring. Both return a pointer to where the match begins (or `NULL` if there's no match — `NULL` pointers are covered in Chapter 11), and subtracting the original string's start address (`char_pos - sentence`) yields the numeric index of the match — a small, legitimate preview of pointer arithmetic, fully explained in Chapter 11.

## Reading Strings: `scanf("%s", ...)` vs. `fgets`

Recall from Chapter 5 that `scanf("%s", ...)` without an explicit width limit risks a buffer overflow, and that it also stops reading at the first whitespace character, so it cannot capture a multi-word line. `fgets` remains the safe default for reading a full line of text, as introduced in Chapter 5:

```c
#include <stdio.h>
#include <string.h>

int main(void) {
    char line[50];

    printf("Enter a full sentence: ");
    fgets(line, sizeof(line), stdin);

    // fgets includes the trailing newline; strip it if present
    size_t len = strlen(line);
    if (len > 0 && line[len - 1] == '\n') {
        line[len - 1] = '\0';
    }

    printf("You entered: \"%s\"\n", line);

    return 0;
}
```

### Compile and run

```bash
gcc -std=c11 -Wall -Wextra fgets_strip_newline.c -o fgets_strip_newline
./fgets_strip_newline
```

### Example interactive session

```text
Enter a full sentence: Hello there, world!
You entered: "Hello there, world!"
```

This pattern — check whether the last character is `'\n'`, and if so overwrite it with `'\0'` — is the standard, idiomatic way to remove the newline that `fgets` (unlike `scanf("%s", ...)`) includes as part of what it reads.

## Manual String Traversal

Writing simple string operations by hand reinforces exactly how the null-terminator convention works underneath the library functions you'll normally just call directly.

### Writing Your Own `strlen`

```c
#include <stdio.h>

int my_strlen(const char str[]) {
    int length = 0;
    while (str[length] != '\0') {
        length++;
    }
    return length;
}

int main(void) {
    char text[] = "Programming";

    printf("Length: %d\n", my_strlen(text));

    return 0;
}
```

### Compile and run

```bash
gcc -std=c11 -Wall -Wextra my_strlen.c -o my_strlen
./my_strlen
```

### Expected output

```text
Length: 11
```

`my_strlen` simply walks forward through the array, counting characters, until it finds the `'\0'` terminator — this is exactly what the real `strlen` does internally. (The `const` keyword in the parameter signals that the function promises not to modify the string through this parameter — a useful, self-documenting habit when a function only needs to read a string, not change it.)

### Reversing a String in Place

```c
#include <stdio.h>
#include <string.h>

void reverse_string(char str[]) {
    int start = 0;
    int end = strlen(str) - 1;

    while (start < end) {
        char temp = str[start];
        str[start] = str[end];
        str[end] = temp;
        start++;
        end--;
    }
}

int main(void) {
    char word[] = "hello";

    reverse_string(word);
    printf("Reversed: %s\n", word);

    return 0;
}
```

### Compile and run

```bash
gcc -std=c11 -Wall -Wextra reverse_string.c -o reverse_string
./reverse_string
```

### Expected output

```text
Reversed: olleh
```

This is structurally identical to the array-reversal algorithm from Chapter 9 — a string is, after all, just a `char` array — except the loop's end boundary is found with `strlen` instead of being passed in directly.

### Checking if a String Is a Palindrome

```c
#include <stdio.h>
#include <string.h>

int is_palindrome(const char str[]) {
    int start = 0;
    int end = strlen(str) - 1;

    while (start < end) {
        if (str[start] != str[end]) {
            return 0; // mismatch found; not a palindrome
        }
        start++;
        end--;
    }
    return 1; // all pairs matched
}

int main(void) {
    char word1[] = "racecar";
    char word2[] = "hello";

    printf("\"%s\" is palindrome? %s\n", word1, is_palindrome(word1) ? "yes" : "no");
    printf("\"%s\" is palindrome? %s\n", word2, is_palindrome(word2) ? "yes" : "no");

    return 0;
}
```

### Compile and run

```bash
gcc -std=c11 -Wall -Wextra palindrome_check.c -o palindrome_check
./palindrome_check
```

### Expected output

```text
"racecar" is palindrome? yes
"hello" is palindrome? no
```

The function compares characters moving inward from both ends simultaneously, returning `0` (false) the instant any pair fails to match, and `1` (true) only if every pair matched all the way to the middle.

## Arrays of Strings

Sometimes you need a whole collection of strings — for example, the names of the days of the week. C offers two distinct ways to represent this.

### Array of `char *` (Pointer Array)

```c
#include <stdio.h>

int main(void) {
    const char *days[7] = {
        "Sunday", "Monday", "Tuesday", "Wednesday",
        "Thursday", "Friday", "Saturday"
    };

    for (int i = 0; i < 7; i++) {
        printf("%d: %s\n", i, days[i]);
    }

    return 0;
}
```

### Compile and run

```bash
gcc -std=c11 -Wall -Wextra days_pointer_array.c -o days_pointer_array
./days_pointer_array
```

### Expected output

```text
0: Sunday
1: Monday
2: Tuesday
3: Wednesday
4: Thursday
5: Friday
6: Saturday
```

`days` is an array of 7 pointers, each one separately pointing at a string literal of whatever length that particular day's name happens to need — `"Wednesday"` and `"Sunday"` can have completely different lengths with no wasted space, because each pointer simply points wherever its literal happens to be stored.

### 2D `char` Array

```c
#include <stdio.h>

int main(void) {
    char days[7][10] = {
        "Sunday", "Monday", "Tuesday", "Wednesday",
        "Thursday", "Friday", "Saturday"
    };

    for (int i = 0; i < 7; i++) {
        printf("%d: %s\n", i, days[i]);
    }

    return 0;
}
```

### Compile and run

```bash
gcc -std=c11 -Wall -Wextra days_2d_array.c -o days_2d_array
./days_2d_array
```

### Expected output

```text
0: Sunday
1: Monday
2: Tuesday
3: Wednesday
4: Thursday
5: Friday
6: Saturday
```

`days[7][10]` reserves one single contiguous block of memory: 7 rows, each a fixed 10 bytes wide (enough for the longest name, `"Wednesday"`, plus its terminator), with shorter names like `"Sunday"` simply wasting some unused bytes at the end of their row.

### Tradeoff

The pointer-array version is more memory-flexible — each string occupies exactly as much space as it needs, with no waste — but it requires the strings already exist somewhere (commonly as literals) for the pointers to point at, and modifying a pointer's target string is subject to the same literal-mutation danger discussed earlier in this chapter. The 2D array version allocates one uniform, contiguous block sized for the *longest* entry, wasting space on shorter ones, but every row is a genuinely independent, freely modifiable `char` array, with no risk of accidentally writing into read-only literal storage.

## Summary

- C strings are `char` arrays terminated by `'\0'`; there is no separate built-in string type, and every string-handling function relies on finding that terminator.
- A string literal's storage is commonly read-only; copying it into a `char` array makes an independent, modifiable copy, while a `char *` pointing directly at a literal must never be written through.
- `<string.h>` provides `strlen`, `strcpy`/`strncpy`, `strcat`/`strncat`, `strcmp`/`strncmp`, `strchr`, and `strstr`; the unbounded forms (`strcpy`, `strcat`) risk buffer overflows, while the bounded forms (`strncpy`, `strncat`) require you to handle null-termination yourself, since they don't guarantee it.
- `fgets` remains the safe default for reading a line of text (recapping Chapter 5); `scanf("%s", ...)` is unsafe without an explicit width limit and cannot read multi-word input.
- Writing `strlen`, string reversal, and a palindrome check by hand reinforces exactly how null-terminator-based traversal works underneath the standard library.
- A collection of strings can be a pointer array (`char *days[]`, memory-efficient, points at existing strings) or a 2D `char` array (`char days[][N]`, one contiguous block, uniform row size, every row independently writable).

## Exercises

1. Write a program that reads a line of text with `fgets`, strips the trailing newline, and prints its length using `strlen`.
2. Write a program that copies one string into another fixed-size buffer using `strncpy`, deliberately testing with a source string longer than the destination, and confirm (by printing) that your manual null-termination line is necessary.
3. Write your own version of `strcmp`, called `my_strcmp`, that returns `0` for equal strings and any nonzero value otherwise (you do not need to match the exact sign convention of the real `strcmp`, just correctly distinguish equal from unequal).
4. Using the days-of-the-week pointer-array example, write a function that takes a day name (read from the user) and prints its index in the array, using `strcmp` in a loop to find the match.
5. *(Harder)* Write a function `void to_uppercase(char str[])` that converts a string to uppercase in place (you may use `toupper` from `<ctype.h>`, or implement the character math yourself), and test it on several strings including ones with mixed case and punctuation.
6. *(Harder)* Write a program that counts how many times each vowel (`a`, `e`, `i`, `o`, `u`, case-insensitive) appears in a line of text read with `fgets`, using manual character-by-character traversal (no `<string.h>` search functions), and prints a small table of the five counts.

---
[← Chapter 9 — Arrays](chapter-09.md) | [Chapter 11 — Pointers: Fundamentals →](chapter-11.md)
