#include <stdio.h>

int main(void) {
    int a = 10;
    int b = 3;

    printf("a = %d, b = %d\n", a, b);
    printf("Addition: %d\n", a + b);
    printf("Subtraction: %d\n", a - b);
    printf("Multiplication: %d\n", a * b);
    printf("Integer division: %d\n", a / b);
    printf("Remainder: %d\n", a % b);

    printf("Is a greater than b? %d\n", a > b);
    printf("Are a and b equal? %d\n", a == b);

    return 0;
}

