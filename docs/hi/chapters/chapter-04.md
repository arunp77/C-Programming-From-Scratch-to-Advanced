# Chapter 4 — Operators और Expressions

Operators symbols होते हैं जो values पर operations perform करते हैं। Expression variables, constants और operators का combination होता है।

## Arithmetic Operators

```c
int a = 10;
int b = 3;

printf("%d\n", a + b);
printf("%d\n", a - b);
printf("%d\n", a * b);
printf("%d\n", a / b);
printf("%d\n", a % b);
```

| Operator | Meaning |
| --- | --- |
| `+` | addition |
| `-` | subtraction |
| `*` | multiplication |
| `/` | division |
| `%` | remainder |

## Integer Division

C में अगर दोनों values `int` हैं, तो division का result भी `int` होगा।

```c
printf("%d\n", 10 / 3);   // 3
```

Decimal result चाहिए तो `float` या `double` use करें।

## Relational Operators

Relational operators comparison करते हैं। Result `1` या `0` होता है।

| Operator | Meaning |
| --- | --- |
| `>` | greater than |
| `<` | less than |
| `>=` | greater than or equal |
| `<=` | less than or equal |
| `==` | equal |
| `!=` | not equal |

## Logical Operators

| Operator | Meaning |
| --- | --- |
| `&&` | AND |
| `||` | OR |
| `!` | NOT |

```c
int age = 20;
int has_id = 1;

if (age >= 18 && has_id) {
    printf("Allowed\n");
}
```

## Practice

1. दो numbers लेकर arithmetic operations print करें।
2. Check करें कि number even है या odd।
3. Age और marks के आधार पर eligibility condition बनाएं।
