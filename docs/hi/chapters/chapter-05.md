# Chapter 5 — Input और Output

Programs useful तब बनते हैं जब वे user से input लेकर output दे सकें। C में basic input/output के लिए `stdio.h`, `printf`, और `scanf` use होते हैं।

## Output with `printf`

```c
printf("Hello\n");
printf("Marks: %d\n", 95);
```

`printf` screen पर text और variable values दिखाता है।

## Input with `scanf`

```c
int age;
printf("Enter age: ");
scanf("%d", &age);
```

`&age` का मतलब है `age` variable का address। `scanf` को address चाहिए ताकि वह input value वहां store कर सके।

## Example Program

```c
#include <stdio.h>

int main(void) {
    int a, b;

    printf("Enter first number: ");
    scanf("%d", &a);

    printf("Enter second number: ");
    scanf("%d", &b);

    printf("Sum = %d\n", a + b);
    return 0;
}
```

## String Input

```c
char name[50];
scanf("%49s", name);
```

Array name already address की तरह काम करता है, इसलिए `&name` नहीं लिखते। `%49s` buffer overflow से बचने में मदद करता है।

## Practice

1. User से name और age लेकर greeting print करें।
2. दो numbers लेकर average calculate करें।
3. Celsius input लेकर Fahrenheit में convert करें।
