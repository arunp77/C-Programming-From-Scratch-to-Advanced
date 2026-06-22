# Chapter 19 — Multi-File Projects और Build Systems

बड़े C projects एक file में नहीं रखे जाते। Code को `.c` और `.h` files में divide किया जाता है।

## Header File

Header file declarations रखती है।

```c
// math_utils.h
#ifndef MATH_UTILS_H
#define MATH_UTILS_H

int add(int a, int b);

#endif
```

## Source File

```c
// math_utils.c
#include "math_utils.h"

int add(int a, int b) {
    return a + b;
}
```

## Main File

```c
#include <stdio.h>
#include "math_utils.h"

int main(void) {
    printf("%d\n", add(2, 3));
    return 0;
}
```

## Compile Multiple Files

```bash
gcc main.c math_utils.c -o app
```

## Makefile

```make
CC = gcc
CFLAGS = -Wall -Wextra -std=c11

app: main.c math_utils.c
	$(CC) $(CFLAGS) main.c math_utils.c -o app
```

## Practice

1. Calculator project को multiple files में divide करें।
2. Header guard use करें।
3. Makefile से project build करें।
