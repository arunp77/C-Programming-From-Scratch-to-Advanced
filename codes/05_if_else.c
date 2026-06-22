#include <stdio.h>

int main(void) {
    int marks;

    printf("Enter marks from 0 to 100: ");
    scanf("%d", &marks);

    if (marks >= 90) {
        printf("Grade: A\n");
    } else if (marks >= 75) {
        printf("Grade: B\n");
    } else if (marks >= 50) {
        printf("Grade: C\n");
    } else {
        printf("Grade: Fail\n");
    }

    return 0;
}

