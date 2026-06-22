# Chapter 0 — Programming और C का परिचय

## Programming क्या है?

Programming का मतलब है computer को clear instructions देना ताकि वह कोई काम कर सके। Computer अपने आप guess नहीं करता। हमें उसे step-by-step बताना पड़ता है कि input क्या है, उस input पर क्या operation करना है, और output कैसे दिखाना है।

Example:

```text
1. User से दो numbers लो
2. दोनों numbers को add करो
3. result screen पर print करो
```

यही basic idea आगे C program में बदलता है।

## C Language क्या है?

C एक powerful, general-purpose programming language है। यह बहुत पुरानी language है, लेकिन आज भी बहुत important है क्योंकि यह हमें computer memory, variables, functions, pointers और system-level programming को deeply समझना सिखाती है।

C का use इन जगहों पर होता है:

- Operating systems
- Embedded systems
- Compilers
- Device drivers
- High-performance applications
- Data structures और algorithms सीखने में

## C क्यों सीखें?

C सीखने से आपको programming की foundation strong मिलती है। जब आप C में memory, pointers और data structures समझ लेते हैं, तो दूसरी languages जैसे C++, Java, Python, Go या Rust समझना आसान हो जाता है।

## एक छोटा C Program

```c
#include <stdio.h>

int main(void) {
    printf("Hello, World!\n");
    return 0;
}
```

## Program कैसे काम करता है?

- `#include <stdio.h>` standard input/output library को include करता है।
- `int main(void)` program की starting point है।
- `printf` screen पर text print करता है।
- `return 0` बताता है कि program successfully finish हुआ।

## याद रखने वाली बात

C में syntax important है। Semicolon, brackets और spelling गलत होने पर compiler error देगा। शुरुआत में errors normal हैं। Error messages पढ़ना भी programming सीखने का हिस्सा है।

Next: [Chapter 1 — Environment Setup](chapter-01.md)
