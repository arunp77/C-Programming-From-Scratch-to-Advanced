#include <stdio.h>

int main(void) {
    int age = 20;
    float height = 5.9f;
    double price = 199.99;
    char grade = 'A';

    printf("Age: %d\n", age);
    printf("Height: %.1f feet\n", height);
    printf("Price: %.2lf\n", price);
    printf("Grade: %c\n", grade);

    return 0;
}

