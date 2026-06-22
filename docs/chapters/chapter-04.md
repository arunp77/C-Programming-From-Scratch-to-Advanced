# Chapter 4 — Operators and Expressions

## Introduction

Variables are only useful if you can combine and manipulate their values. An **operator** is a symbol that performs an operation on one or more values, such as adding two numbers or comparing two numbers for equality. An **expression** is any combination of values, variables, and operators that produces a single resulting value. This chapter covers C's most important operators and the rules for how they combine.

## Arithmetic Operators

C provides the five basic arithmetic operators: `+` (addition), `-` (subtraction), `*` (multiplication), `/` (division), and `%` (remainder, also called **modulo**).

The trickiest of these for beginners is `/` (division), because it behaves differently depending on the types involved. When both operands (the values being operated on) are integers, `/` performs **integer division**: it computes the whole-number quotient and discards (truncates) any fractional part entirely — it does not round. When at least one operand is a floating-point type (`float` or `double`), `/` performs ordinary floating-point division, keeping the fractional part.

```c
#include <stdio.h>

int main(void) {
    int a = 7;
    int b = 2;

    printf("Integer division: 7 / 2 = %d\n", a / b);
    printf("Floating-point division: 7.0 / 2 = %f\n", 7.0 / b);
    printf("Remainder: 7 %% 2 = %d\n", a % b);

    return 0;
}
```

### Compile and run

```bash
gcc -std=c11 -Wall -Wextra arithmetic.c -o arithmetic
./arithmetic
```

### Expected output

```text
Integer division: 7 / 2 = 3
Floating-point division: 7.0 / 2 = 3.500000
Remainder: 7 % 2 = 1
```

Notice `7 / 2` gives `3`, not `3.5` and not `4` — the `.5` is simply thrown away because both `7` and `2` are `int`. To get a fractional result, at least one operand must be a floating-point value, which is why `7.0 / 2` (note the `.0`) gives `3.500000`. Also notice that printing a literal `%` character with `printf` requires writing it twice, `%%`, since a single `%` would otherwise be read as the start of a format specifier — this is a small but common gotcha that we'll revisit in Chapter 5.

!!! warning "Division by zero"
    Dividing an integer by zero (`5 / 0`) is undefined behavior in C and typically crashes the program immediately. Dividing a floating-point value by zero (`5.0 / 0.0`) does not crash — it produces a special "infinity" value instead, governed by the floating-point standard most systems use. Be careful never to divide an integer by a variable that might be zero without checking first; we'll cover the `if` statement needed to guard against this in Chapter 6.

## Relational and Logical Operators

**Relational operators** compare two values and produce a `bool`-like result: `1` for true, `0` for false (recall from Chapter 3 that C's `_Bool`/`bool` type is really just a restricted integer). The relational operators are `==` (equal to), `!=` (not equal to), `<` (less than), `>` (greater than), `<=` (less than or equal to), and `>=` (greater than or equal to).

!!! warning "`=` vs. `==`"
    A single `=` is the *assignment* operator (it stores a value into a variable). A double `==` is the *equality comparison* operator (it checks whether two values are equal, without changing either one). Writing `if (x = 5)` when you meant `if (x == 5)` is one of the most notorious beginner mistakes in C, because `x = 5` is itself a valid expression (it assigns 5 to x and then evaluates to 5, which counts as "true"), so the compiler may only warn, not error. Always double-check you've used `==` when comparing.

**Logical operators** combine or invert true/false values: `&&` (logical AND — true only if both sides are true), `||` (logical OR — true if at least one side is true), and `!` (logical NOT — inverts true to false and vice versa).

```c
#include <stdio.h>
#include <stdbool.h>

int main(void) {
    int age = 20;
    bool has_id = true;

    printf("age > 18: %d\n", age > 18);
    printf("age == 20 && has_id: %d\n", age == 20 && has_id);
    printf("age < 18 || has_id: %d\n", age < 18 || has_id);
    printf("!has_id: %d\n", !has_id);

    return 0;
}
```

### Compile and run

```bash
gcc -std=c11 -Wall -Wextra logic.c -o logic
./logic
```

### Expected output

```text
age > 18: 1
age == 20 && has_id: 1
age < 18 || has_id: 1
!has_id: 0
```

Here is a small truth table illustrating `&&` and `||` for all combinations of two values, `p` and `q`:

| `p` | `q` | `p && q` | `p \|\| q` |
|---|---|---|---|
| true | true | true | true |
| true | false | false | true |
| false | true | false | true |
| false | false | false | false |

## Assignment and Compound Assignment

The plain assignment operator `=` stores a value into a variable, as you've already seen. C also provides **compound assignment operators** that combine an arithmetic operation with assignment in one step: `+=`, `-=`, `*=`, `/=`, and `%=`. The statement `x += 5;` means exactly the same thing as `x = x + 5;`, just written more compactly.

```c
#include <stdio.h>

int main(void) {
    int score = 10;

    score += 5;   // same as: score = score + 5;
    printf("After += 5: %d\n", score);

    score -= 3;   // same as: score = score - 3;
    printf("After -= 3: %d\n", score);

    score *= 2;   // same as: score = score * 2;
    printf("After *= 2: %d\n", score);

    score /= 4;   // same as: score = score / 4;
    printf("After /= 4: %d\n", score);

    return 0;
}
```

### Compile and run

```bash
gcc -std=c11 -Wall -Wextra compound.c -o compound
./compound
```

### Expected output

```text
After += 5: 15
After -= 3: 12
After *= 2: 24
After /= 4: 6
```

## Increment and Decrement: Prefix vs. Postfix

`++` increases a variable's value by 1; `--` decreases it by 1. Each comes in two forms: **prefix** (`++x`) and **postfix** (`x++`). Both forms ultimately increase `x` by 1, but they differ in what value the *expression itself* produces when used inside a larger expression, such as inside a `printf` call.

- **Prefix (`++x`)**: increases `x` first, *then* the expression evaluates to the new (already incremented) value.
- **Postfix (`x++`)**: the expression evaluates to the *original* value first, and *then* `x` is increased.

To see the difference clearly and safely, we will print each form separately, with its own dedicated variable:

```c
#include <stdio.h>

int main(void) {
    int a = 5;
    int b = 5;

    printf("Postfix: a++ evaluates to %d\n", a++);
    printf("After postfix, a is now: %d\n", a);

    printf("Prefix: ++b evaluates to %d\n", ++b);
    printf("After prefix, b is now: %d\n", b);

    return 0;
}
```

### Compile and run

```bash
gcc -std=c11 -Wall -Wextra incdec.c -o incdec
./incdec
```

### Expected output

```text
Postfix: a++ evaluates to 5
After postfix, a is now: 6
Prefix: ++b evaluates to 6
After prefix, b is now: 6
```

Notice that `a++` printed `5` (the value *before* incrementing), while `++b` printed `6` (the value *after* incrementing) — yet both `a` and `b` end up at `6` afterward. The only difference is what value the expression itself produces at the moment it's evaluated.

!!! danger "Never combine multiple side effects on the same variable in one expression"
    You might be tempted to write something like `printf("%d %d", a++, ++a);` to "see both at once." Do not do this. Modifying the same variable more than once within a single expression, where the order of those modifications isn't otherwise sequenced, is **undefined behavior** in C — the standard does not guarantee what order the modifications happen in, so different compilers (or even the same compiler with different optimization settings) may legitimately produce different results. This is precisely why the example above uses two separate variables (`a` and `b`) and two separate statements, rather than combining both operations on one variable in a single expression. As a general rule throughout this tutorial: never read and modify the same variable more than once within one unsequenced expression.

## Bitwise Operators

**Bitwise operators** work directly on the individual binary digits (bits) that make up an integer's representation in memory, rather than treating the number as a whole arithmetic quantity. Recall from Chapter 3 that every integer is stored as a sequence of bits, each either 0 or 1. The bitwise operators are: `&` (AND), `|` (OR), `^` (XOR, exclusive-or), `~` (NOT, inverts every bit), `<<` (left shift), and `>>` (right shift).

To make the binary patterns visible, the example below uses small `unsigned char` values (1 byte, 8 bits each) and prints them as binary text using a small helper loop. (We have not covered loops yet — they are the subject of Chapter 6 — so for this one example, treat the loop itself as a black box whose job is simply "print these 8 bits"; focus on the bitwise results, not the loop mechanics.)

```c
#include <stdio.h>

void print_binary(unsigned char value) {
    for (int i = 7; i >= 0; i--) {
        printf("%d", (value >> i) & 1);
    }
}

int main(void) {
    unsigned char a = 0b00001100; // 12
    unsigned char b = 0b00001010; // 10

    printf("a       = "); print_binary(a); printf("\n");
    printf("b       = "); print_binary(b); printf("\n");

    printf("a & b   = "); print_binary(a & b); printf("\n");
    printf("a | b   = "); print_binary(a | b); printf("\n");
    printf("a ^ b   = "); print_binary(a ^ b); printf("\n");
    printf("~a      = "); print_binary(~a); printf("\n");
    printf("a << 2  = "); print_binary(a << 2); printf("\n");
    printf("a >> 2  = "); print_binary(a >> 2); printf("\n");

    return 0;
}
```

### Compile and run

```bash
gcc -std=c11 -Wall -Wextra bitwise.c -o bitwise
./bitwise
```

### Expected output

```text
a       = 00001100
b       = 00001010
a & b   = 00001000
a | b   = 00001110
a ^ b   = 00000110
~a      = 11110011
a << 2  = 00110000
a >> 2  = 00000011
```

Reading these results: `&` sets a bit to 1 only where *both* inputs have a 1 (only bit position 3 has 1 in both `a` and `b`). `|` sets a bit to 1 where *either* input has a 1. `^` sets a bit to 1 where the inputs *differ*. `~` flips every single bit. `<<` shifts every bit two places to the left (which, for unsigned values, is equivalent to multiplying by 4 — that is, 2²), discarding bits that fall off the left edge and filling with zeros on the right. `>>` shifts every bit two places to the right (equivalent here to integer division by 4), filling with zeros on the left for unsigned types.

!!! note "We are not using functions or loops yet"
    The `print_binary` function and the `for` loop inside it are here only so you can *see* the bitwise results. We will properly explain loops in Chapter 6 and functions in Chapter 8. If the loop syntax looks unfamiliar, that's expected — just trust that it prints 8 binary digits, and focus on the bitwise operator results themselves.

## The Ternary Conditional Operator

The **ternary operator**, `condition ? value_if_true : value_if_false`, is C's only operator that takes three operands (hence "ternary"). It evaluates `condition`; if it's true, the whole expression evaluates to `value_if_true`, otherwise to `value_if_false`. It is a compact alternative to a full `if`/`else` statement (covered in Chapter 6) when you just need to pick between two values.

```c
#include <stdio.h>

int main(void) {
    int temperature = 15;

    const char *description = (temperature > 20) ? "warm" : "cool";
    printf("It is %s today.\n", description);

    return 0;
}
```

### Compile and run

```bash
gcc -std=c11 -Wall -Wextra ternary.c -o ternary
./ternary
```

### Expected output

```text
It is cool today.
```

(The `const char *` type used here to hold a piece of text is a pointer to characters — full detail comes in Chapter 11. For now, just know it's how C represents "a piece of text that this variable refers to.")

## Operator Precedence and Associativity

When an expression contains multiple operators, **precedence** determines which operator is evaluated first, and **associativity** determines the order in which operators of *equal* precedence are grouped (left-to-right or right-to-left). This is directly analogous to the "order of operations" (PEMDAS/BODMAS) you may have encountered in school arithmetic, extended to cover C's much larger set of operators.

Consider:

```c
#include <stdio.h>

int main(void) {
    int result1 = 2 + 3 * 4;          // multiplication before addition
    int result2 = (2 + 3) * 4;        // parentheses override precedence
    int result3 = 10 - 2 - 3;         // left-to-right associativity for '-'

    printf("2 + 3 * 4 = %d\n", result1);
    printf("(2 + 3) * 4 = %d\n", result2);
    printf("10 - 2 - 3 = %d\n", result3);

    return 0;
}
```

### Compile and run

```bash
gcc -std=c11 -Wall -Wextra precedence.c -o precedence
./precedence
```

### Expected output

```text
2 + 3 * 4 = 14
(2 + 3) * 4 = 20
10 - 2 - 3 = 5
```

`2 + 3 * 4` is `14`, not `20`, because `*` has higher precedence than `+`, so `3 * 4` is computed first (`12`), then `2 + 12 = 14`. Wrapping `2 + 3` in parentheses forces it to be computed first regardless of precedence, giving `20`. And `10 - 2 - 3` shows associativity: `-` groups left-to-right, so this is `(10 - 2) - 3 = 5`, not `10 - (2 - 3) = 11`.

A trickier case worth knowing about now: bitwise operators have *lower* precedence than relational operators, which surprises many beginners.

```c
#include <stdio.h>

int main(void) {
    // Intention: check whether exactly one of these conditions is true.
    // Mistake: '==' binds tighter than '&', so this doesn't do what it looks like.
    int wrong = 1 & 1 == 1;

    // Correct: force the comparison to happen first using parentheses.
    int right = (1 & 1) == 1;

    printf("wrong = %d\n", wrong);
    printf("right = %d\n", right);

    return 0;
}
```

### Compile and run

```bash
gcc -std=c11 -Wall -Wextra tricky.c -o tricky
./tricky
```

### Expected output

```text
wrong = 1
right = 1
```

In this particular case both happen to come out the same, which is exactly what makes this kind of mistake dangerous — it can silently produce a "correct-looking" result for some inputs while being wrong in general. The lesson is not to memorize every precedence level, but to **use parentheses liberally** any time you mix different categories of operators (especially bitwise with relational/logical), so your intent is unambiguous to both the compiler and to anyone reading your code. The complete, authoritative precedence and associativity table for every C operator is provided in [Appendix B](../chapters/Appendix-B.md) — refer to it whenever you're unsure, rather than trying to memorize it up front.

## Type Conversion

**Implicit conversion** (also called **promotion** in some contexts) happens automatically when an expression mixes different types — C converts values as needed to make the operation possible, following a set of standard rules. The most common case: when an expression mixes an integer and a floating-point value, the integer is automatically converted to floating-point before the operation, which is exactly why `7.0 / 2` (mixing `double` and `int`) produces a floating-point result, as we saw earlier in this chapter.

```c
#include <stdio.h>

int main(void) {
    int whole = 7;
    double fraction = whole / 2.0;  // 'whole' is implicitly converted to double

    printf("%f\n", fraction);

    return 0;
}
```

### Compile and run

```bash
gcc -std=c11 -Wall -Wextra implicit.c -o implicit
./implicit
```

### Expected output

```text
3.500000
```

**Explicit conversion**, called **casting**, is when you, the programmer, deliberately tell the compiler to treat a value as a different type, by writing the target type in parentheses before the value: `(double)`, `(int)`, and so on. This is necessary, for example, when you want floating-point division between two variables that are both declared as `int`:

```c
#include <stdio.h>

int main(void) {
    int a = 7;
    int b = 2;

    double result = (double)a / b;  // cast 'a' to double before dividing
    printf("%f\n", result);

    double precise = 9.99;
    int truncated = (int)precise;   // explicit cast, precision is lost
    printf("%d\n", truncated);

    return 0;
}
```

### Compile and run

```bash
gcc -std=c11 -Wall -Wextra cast.c -o cast
./cast
```

### Expected output

```text
3.500000
9
```

The cast `(double)a` converts `a` to `3.5`-capable floating point *before* the division happens, so the whole expression becomes floating-point division, giving `3.5` rather than the truncated `3`. The second cast, `(int)precise`, deliberately throws away the fractional part of `9.99`, leaving `9` — this is called **precision loss**, and it is something you, the programmer, are explicitly asking for when you write a cast like this. Casts are a deliberate, visible signal of intent; use them when you genuinely mean to convert a value, rather than relying on implicit conversion rules you may not remember precisely.

## `sizeof` Revisited

Chapter 3 introduced `sizeof` as a way to check how many bytes a type occupies. It's worth restating here that `sizeof` is technically an **operator**, evaluated entirely at compile time (the compiler already knows the size of every type before your program ever runs) — it does not "do work" while your program executes the way `+` or `*` do. You can apply it to a type name (`sizeof(int)`) or directly to a variable (`sizeof(my_variable)`), and both forms appear throughout the rest of this tutorial, especially once we reach arrays and dynamic memory allocation.

## Summary

- Arithmetic operators `+ - * / %`: integer division truncates toward zero with no rounding; mixing in a floating-point operand produces a floating-point result.
- Relational operators (`== != < > <= >=`) compare values; logical operators (`&& || !`) combine or invert true/false results. Never confuse `=` (assignment) with `==` (comparison).
- Compound assignment operators (`+= -= *= /= %=`) combine an operation with assignment in one step.
- `++`/`--` come in prefix and postfix forms that differ in what value the expression itself produces; never apply more than one side-effecting operator to the same variable within a single unsequenced expression, since that is undefined behavior.
- Bitwise operators (`& | ^ ~ << >>`) operate on individual bits, distinct from arithmetic and logical operators.
- The ternary operator `condition ? a : b` is a compact way to choose between two values.
- Operator precedence and associativity determine evaluation order; use parentheses generously, especially when mixing categories of operators, and consult Appendix B for the full table.
- Implicit conversion happens automatically when types are mixed in an expression; explicit casting (`(type)value`) lets you deliberately convert a value, sometimes at the cost of precision.
- `sizeof` is a compile-time operator that reports a type's or variable's size in bytes.

## Exercises

1. Write a program that computes and prints the integer division and remainder of two `int` variables you choose, then separately computes their floating-point division by casting one of them to `double`.
2. Write truth-table-style output for `&&`, `||`, and `!` by printing the result of each combination of two `bool` variables (similar to the table in this chapter), and confirm your program's output matches the table.
3. Using two separate variables, demonstrate the difference between prefix `++x` and postfix `x++` the way this chapter did, but with `--` instead.
4. Pick two small positive `unsigned char` values, predict the binary result of applying `&`, `|`, and `^` to them by hand, then verify your prediction using a program structured like the bitwise example in this chapter.
5. *(Harder)* Write an expression mixing relational and bitwise operators without parentheses (similar to the `1 & 1 == 1` example), predict its result by looking up precedence in Appendix B, then verify by running it. Add parentheses to make the intended meaning explicit and confirm the result is unchanged (or changed, if your original expression was actually a bug).
6. *(Harder)* Write a program that adds a `float` and an `int` together and prints the result, then explain, referencing this chapter's implicit conversion rules, exactly what type the addition happens in and why. Then deliberately cast the result to `int` and explain what value is lost.

---
[← Chapter 3 — Variables, Data Types and Constants](chapter-03.md) | [Chapter 5 — Input and Output →](chapter-05.md)
