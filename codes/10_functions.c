#include <stdio.h>

int add(int a, int b) {
    return a + b;
}

int square(int number) {
    return number * number;
}

int main(void) {
    int x = 4;
    int y = 7;

    printf("%d + %d = %d\n", x, y, add(x, y));
    printf("Square of %d = %d\n", x, square(x));

    return 0;
}

