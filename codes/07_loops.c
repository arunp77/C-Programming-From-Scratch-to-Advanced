#include <stdio.h>

int main(void) {
    int i;
    int count = 1;

    printf("For loop:\n");
    for (i = 1; i <= 5; i++) {
        printf("%d ", i);
    }
    printf("\n");

    printf("While loop:\n");
    while (count <= 5) {
        printf("%d ", count);
        count++;
    }
    printf("\n");

    printf("Do while loop:\n");
    do {
        printf("This runs at least once.\n");
    } while (0);

    return 0;
}

