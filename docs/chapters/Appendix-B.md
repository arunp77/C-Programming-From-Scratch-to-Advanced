# Appendix B — Cheat Sheets

Quick-reference tables for syntax and details that are useful to look up repeatedly but not worth memorizing word for word.

## Operator Precedence and Associativity

Listed from highest precedence (evaluated first) to lowest. Operators in the same row group share precedence; associativity determines evaluation order among operators of equal precedence in the same expression.

| Precedence | Operators | Description | Associativity |
|---|---|---|---|
| 1 (highest) | `()` `[]` `.` `->` `++` `--` (postfix) | Function call, array subscript, member access, postfix increment/decrement | Left to right |
| 2 | `++` `--` (prefix) `+` `-` (unary) `!` `~` `*` (deref) `&` (address-of) `sizeof` `(type)` (cast) | Prefix increment/decrement, unary plus/minus, logical/bitwise NOT, dereference, address-of, sizeof, cast | Right to left |
| 3 | `*` `/` `%` | Multiplication, division, modulo | Left to right |
| 4 | `+` `-` | Addition, subtraction | Left to right |
| 5 | `<<` `>>` | Bitwise shift left/right | Left to right |
| 6 | `<` `<=` `>` `>=` | Relational comparisons | Left to right |
| 7 | `==` `!=` | Equality comparisons | Left to right |
| 8 | `&` | Bitwise AND | Left to right |
| 9 | `^` | Bitwise XOR | Left to right |
| 10 | `\|` | Bitwise OR | Left to right |
| 11 | `&&` | Logical AND | Left to right |
| 12 | `\|\|` | Logical OR | Left to right |
| 13 | `?:` | Ternary conditional | Right to left |
| 14 | `=` `+=` `-=` `*=` `/=` `%=` `&=` `^=` `\|=` `<<=` `>>=` | Assignment and compound assignment | Right to left |
| 15 (lowest) | `,` | Comma operator | Left to right |

!!! tip "When in doubt, parenthesize"
    Relying on precedence rules for anything beyond the most common cases (arithmetic before comparison, comparison before logical AND/OR) makes code harder to read. Adding explicit parentheses costs nothing and removes any ambiguity for the reader, even when the compiler does not strictly need them.

## `printf` Format Specifier Reference

A conversion specification has the general form `%[flags][width][.precision][length]conversion`.

**Flags**

| Flag | Effect |
|---|---|
| `-` | Left-justify within the given field width (default is right-justify). |
| `+` | Always show a sign (`+` or `-`) for signed numeric conversions. |
| `0` | Pad numeric output with leading zeros instead of spaces. |
| `space` | Insert a space before a positive number's value, where `+` would go. |
| `#` | Alternate form: e.g. adds `0x`/`0X` prefix for `%x`/`%X`, ensures a decimal point for `%f`. |

**Width and precision**

| Form | Meaning |
|---|---|
| `n` (a number) | Minimum field width, padded with spaces (or zeros with the `0` flag). |
| `*` | Width supplied as an extra `int` argument instead of being hardcoded. |
| `.n` | Precision: max characters for `%s`, digits after the decimal point for `%f`/`%e`, minimum digits for integer conversions. |

**Length modifiers**

| Modifier | Applies to | Meaning |
|---|---|---|
| `h` | `d, i, o, u, x, X` | Argument is `short`. |
| `hh` | `d, i, o, u, x, X` | Argument is `char`. |
| `l` | `d, i, o, u, x, X` | Argument is `long`. |
| `ll` | `d, i, o, u, x, X` | Argument is `long long`. |
| `z` | `d, i, o, u, x, X` | Argument is `size_t`-sized. |
| `L` | `f, e, g, a` | Argument is `long double`. |

**Conversion characters**

| Conversion | Type | Description |
|---|---|---|
| `%d`, `%i` | `int` | Signed decimal integer. |
| `%u` | `unsigned int` | Unsigned decimal integer. |
| `%o` | `unsigned int` | Unsigned octal. |
| `%x`, `%X` | `unsigned int` | Unsigned hexadecimal, lowercase/uppercase letters. |
| `%f` | `double` | Fixed-point decimal notation. |
| `%e`, `%E` | `double` | Scientific notation, lowercase/uppercase exponent marker. |
| `%g`, `%G` | `double` | Shorter of `%f`/`%e` representation, trailing zeros trimmed. |
| `%c` | `int` (as `char`) | Single character. |
| `%s` | `char *` | Null-terminated string. |
| `%p` | `void *` | Pointer value, implementation-defined format (typically hex address). |
| `%%` | — | A literal `%` character; takes no argument. |

## `scanf` Format Specifier Reference

| Conversion | Expects argument type | Description |
|---|---|---|
| `%d` | `int *` | Reads a signed decimal integer. |
| `%u` | `unsigned int *` | Reads an unsigned decimal integer. |
| `%f` | `float *` | Reads a floating-point value. |
| `%lf` | `double *` | Reads a `double`; note `%f` is for `float` only when reading with `scanf`. |
| `%c` | `char *` | Reads a single character, including whitespace (does not skip it). |
| `%s` | `char *` | Reads a whitespace-delimited token into a buffer; does not bound-check the buffer size on its own. |
| `%x` | `unsigned int *` | Reads a hexadecimal integer. |
| `%%` | — | Matches a literal `%` in the input. |
| ` ` (space) in format string | — | Skips any amount of leading whitespace, including none — commonly used before `%c` to skip a leftover newline. |

!!! warning "Bounding %s reads"
    `scanf("%s", buffer)` performs no bounds checking and will overflow `buffer` given sufficiently long input. Always provide a width limit, e.g. `scanf("%31s", buffer)` for a 32-byte buffer (reserving one byte for the null terminator), or prefer `fgets` followed by parsing for untrusted input.

## Data Type Sizes and Ranges

Typical sizes on a common 64-bit platform (Linux x86-64, gcc, LP64 data model). **Sizes are implementation-defined** — the C standard only guarantees *minimums*, not exact sizes. `sizeof` and the constants in `<limits.h>` (for integer types) or `<float.h>` (for floating-point types) are the only fully portable way to know actual sizes and ranges on any given platform.

| Type | Typical size (bytes) | Typical range |
|---|---|---|
| `char` | 1 | `-128` to `127` (signed) or `0` to `255` (unsigned); signedness of plain `char` is implementation-defined |
| `short` | 2 | `-32,768` to `32,767` |
| `int` | 4 | `-2,147,483,648` to `2,147,483,647` |
| `unsigned int` | 4 | `0` to `4,294,967,295` |
| `long` | 8 | `-9,223,372,036,854,775,808` to `9,223,372,036,854,775,807` |
| `long long` | 8 | Same range as `long` on this platform; standard guarantees at least the same minimum range as `long` |
| `float` | 4 | ~`1.2E-38` to ~`3.4E38`, about 6–7 significant decimal digits |
| `double` | 8 | ~`2.3E-308` to ~`1.7E308`, about 15–16 significant decimal digits |
| `long double` | 16 (platform-dependent; sometimes 8 or 10) | Extended range and precision beyond `double`, varies significantly by platform |
| `int *` (any pointer) | 8 | N/A — holds a memory address |

For portable, exact-width integer types regardless of platform, `<stdint.h>` provides types such as `int8_t`, `int16_t`, `int32_t`, `int64_t` (and `uint8_t`, `uint16_t`, `uint32_t`, `uint64_t` for unsigned versions) — each guaranteed to be exactly that many bits wide on any conforming platform, which plain `int`/`long` do not guarantee.

## Common gcc Compiler Flags

| Flag | Effect |
|---|---|
| `-o <name>` | Sets the output file name for the compiled program. |
| `-c` | Compiles to an object file (`.o`) without linking. |
| `-Wall` | Enables a broad set of commonly useful warnings. |
| `-Wextra` | Enables additional warnings beyond `-Wall`. |
| `-Wpedantic` | Warns about non-standard language extensions; strict ISO C compliance checking. |
| `-Werror` | Treats all warnings as hard errors, stopping compilation. |
| `-g` | Embeds debug information for use with `gdb`. |
| `-std=c11` | Selects the C11 language standard explicitly. |
| `-O0` | No optimization (default); fastest compilation, best debugging fidelity. |
| `-O2` | A standard, fairly aggressive optimization level; also enables some additional compiler warning analyses. |
| `-lm` | Links the math library (needed for some `<math.h>` functions on some systems). |
| `-I<dir>` | Adds `<dir>` to the header search path. |
| `-L<dir>` | Adds `<dir>` to the library search path (for linking). |
| `-l<name>` | Links against library `lib<name>.a` or `lib<name>.so`. |

## Printable ASCII Table (32–126)

| Dec | Char | Dec | Char | Dec | Char | Dec | Char |
|---|---|---|---|---|---|---|---|
| 32 | space | 56 | `8` | 80 | `P` | 104 | `h` |
| 33 | `!` | 57 | `9` | 81 | `Q` | 105 | `i` |
| 34 | `"` | 58 | `:` | 82 | `R` | 106 | `j` |
| 35 | `#` | 59 | `;` | 83 | `S` | 107 | `k` |
| 36 | `$` | 60 | `<` | 84 | `T` | 108 | `l` |
| 37 | `%` | 61 | `=` | 85 | `U` | 109 | `m` |
| 38 | `&` | 62 | `>` | 86 | `V` | 110 | `n` |
| 39 | `'` | 63 | `?` | 87 | `W` | 111 | `o` |
| 40 | `(` | 64 | `@` | 88 | `X` | 112 | `p` |
| 41 | `)` | 65 | `A` | 89 | `Y` | 113 | `q` |
| 42 | `*` | 66 | `B` | 90 | `Z` | 114 | `r` |
| 43 | `+` | 67 | `C` | 91 | `[` | 115 | `s` |
| 44 | `,` | 68 | `D` | 92 | `\` | 116 | `t` |
| 45 | `-` | 69 | `E` | 93 | `]` | 117 | `u` |
| 46 | `.` | 70 | `F` | 94 | `^` | 118 | `v` |
| 47 | `/` | 71 | `G` | 95 | `_` | 119 | `w` |
| 48 | `0` | 72 | `H` | 96 | `` ` `` | 120 | `x` |
| 49 | `1` | 73 | `I` | 97 | `a` | 121 | `y` |
| 50 | `2` | 74 | `J` | 98 | `b` | 122 | `z` |
| 51 | `3` | 75 | `K` | 99 | `c` | 123 | `{` |
| 52 | `4` | 76 | `L` | 100 | `d` | 124 | `\|` |
| 53 | `5` | 77 | `M` | 101 | `e` | 125 | `}` |
| 54 | `6` | 78 | `N` | 102 | `f` | 126 | `~` |
| 55 | `7` | 79 | `O` | 103 | `g` | | |

---
[← Appendix A — C Standard Library Reference](Appendix-A.md) | [Appendix C — Practice Projects and Next Steps →](Appendix-C.md)
