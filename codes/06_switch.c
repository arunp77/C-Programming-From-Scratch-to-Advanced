#include <stdio.h>

int main(void) {
    int choice;

    printf("Choose an option:\n");
    printf("1. Say hello\n");
    printf("2. Show today topic\n");
    printf("3. Exit\n");
    printf("Enter choice: ");
    scanf("%d", &choice);

    switch (choice) {
    case 1:
        printf("Hello!\n");
        break;
    case 2:
        printf("Today's topic is switch statements.\n");
        break;
    case 3:
        printf("Goodbye!\n");
        break;
    default:
        printf("Invalid choice.\n");
        break;
    }

    return 0;
}

