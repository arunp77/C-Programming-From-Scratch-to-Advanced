# Chapter 8 — Functions और Modular Programming

Function reusable block of code होता है। इससे program छोटा, readable और maintainable बनता है।

## Function क्यों use करें?

- Same code बार-बार लिखने से बचता है।
- Program parts में divide होता है।
- Testing आसान होती है।

## Function Example

```c
#include <stdio.h>

int add(int a, int b) {
    return a + b;
}

int main(void) {
    int result = add(5, 3);
    printf("Result = %d\n", result);
    return 0;
}
```

## Function Parts

```c
int add(int a, int b)
```

- `int` return type है।
- `add` function name है।
- `int a, int b` parameters हैं।

## `void` Functions

अगर function कुछ return नहीं करता:

```c
void greet(void) {
    printf("Hello\n");
}
```

## Practice

1. `square` function लिखें।
2. `is_even` function लिखें जो 1 या 0 return करे।
3. Calculator program में operations को अलग functions में रखें।
