#include <stdio.h>
#include <stdlib.h>

int main(void) {
    int count;
    int *numbers;
    int i;
    int sum = 0;

    printf("How many numbers? ");
    scanf("%d", &count);

    if (count <= 0) {
        printf("Count must be positive.\n");
        return 1;
    }

    numbers = malloc((size_t)count * sizeof(int));
    if (numbers == NULL) {
        printf("Memory allocation failed.\n");
        return 1;
    }

    for (i = 0; i < count; i++) {
        printf("Enter number %d: ", i + 1);
        scanf("%d", &numbers[i]);
        sum += numbers[i];
    }

    printf("Sum = %d\n", sum);

    free(numbers);
    return 0;
}

