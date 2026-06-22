# Chapter 15 — Preprocessor Directives और Macros

Preprocessor compilation से पहले source code process करता है। Lines जो `#` से शुरू होती हैं, preprocessor directives कहलाती हैं।

## `#include`

```c
#include <stdio.h>
```

यह header file include करता है।

## `#define`

```c
#define PI 3.14159
#define MAX_SIZE 100
```

Macro symbolic name देता है।

## Function-like Macro

```c
#define SQUARE(x) ((x) * (x))
```

Parentheses important हैं ताकि operator precedence bugs न आएं।

## Conditional Compilation

```c
#ifdef DEBUG
printf("Debug mode\n");
#endif
```

यह code को conditionally include करता है।

## Header Guards

Header files में duplicate inclusion रोकने के लिए:

```c
#ifndef STUDENT_H
#define STUDENT_H

struct Student {
    int roll_no;
};

#endif
```

## Practice

1. `PI` macro बनाकर circle area calculate करें।
2. `MAX(a, b)` macro लिखें।
3. Simple header file बनाकर header guard use करें।
