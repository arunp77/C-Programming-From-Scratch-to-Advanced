# Chapter 3 — Variables, Data Types और Constants

इस chapter में हम सीखेंगे कि C में data को memory में कैसे store किया जाता है। Programming में लगभग हर काम data से शुरू होता है, इसलिए `variable` और `data type` समझना बहुत जरूरी है।

## Variable क्या है?

`variable` memory में एक named space होता है जहां value store की जाती है। Value program चलते समय change हो सकती है।

```c
int age = 20;
float height = 5.9f;
char grade = 'A';
```

यहां `age`, `height`, और `grade` variables हैं।

## Data Type क्या है?

`data type` compiler को बताता है कि variable में किस तरह का data रखा जाएगा और memory कितनी चाहिए।

| Type | Use | Example |
| --- | --- | --- |
| `int` | whole numbers | `10`, `-5` |
| `float` | decimal values | `3.14f` |
| `double` | larger decimal values | `99.99` |
| `char` | single character | `'A'` |

## Example Program

```c
#include <stdio.h>

int main(void) {
    int age = 20;
    float height = 5.9f;
    char grade = 'A';

    printf("Age: %d\n", age);
    printf("Height: %.1f\n", height);
    printf("Grade: %c\n", grade);

    return 0;
}
```

## Format Specifiers

`printf` को बताना पड़ता है कि किस type की value print करनी है।

| Specifier | Type |
| --- | --- |
| `%d` | `int` |
| `%f` | `float` / `double` |
| `%c` | `char` |
| `%s` | string |

## Constants

`constant` ऐसी value होती है जिसे program में change नहीं करना चाहिए।

```c
const int max_marks = 100;
```

`const` use करने से compiler accidental changes पकड़ सकता है।

## Practice

1. अपना नाम, age और grade variables में store करके print करें।
2. `int`, `float`, और `char` के लिए अलग-अलग examples बनाएं।
3. `const float pi = 3.14159f;` use करके circle area calculate करें।
