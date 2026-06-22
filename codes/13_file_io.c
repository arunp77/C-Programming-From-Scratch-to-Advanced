#include <stdio.h>

int main(void) {
    FILE *file;
    char name[50];
    int age;

    printf("Enter name: ");
    scanf("%49s", name);

    printf("Enter age: ");
    scanf("%d", &age);

    file = fopen("student.txt", "w");
    if (file == NULL) {
        printf("Could not open file for writing.\n");
        return 1;
    }

    fprintf(file, "Name: %s\nAge: %d\n", name, age);
    fclose(file);

    printf("Data written to student.txt\n");

    return 0;
}

