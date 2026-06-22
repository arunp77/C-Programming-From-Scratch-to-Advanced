#include <stdio.h>
#include <string.h>

int main(void) {
    char first_name[30];
    char last_name[30];
    char full_name[70];

    printf("Enter first name: ");
    scanf("%29s", first_name);

    printf("Enter last name: ");
    scanf("%29s", last_name);

    strcpy(full_name, first_name);
    strcat(full_name, " ");
    strcat(full_name, last_name);

    printf("Full name: %s\n", full_name);
    printf("Length: %zu characters\n", strlen(full_name));

    return 0;
}

