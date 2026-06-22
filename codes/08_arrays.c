#include <stdio.h>

int main(void) {
    int numbers[5];
    int sum = 0;
    int i;

    for (i = 0; i < 5; i++) {
        printf("Enter number %d: ", i + 1);
        scanf("%d", &numbers[i]);
        sum += numbers[i];
    }

    printf("You entered: ");
    for (i = 0; i < 5; i++) {
        printf("%d ", numbers[i]);
    }

    printf("\nSum = %d\n", sum);

    return 0;
}

