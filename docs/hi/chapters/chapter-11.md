# Chapter 11 — Pointers: Fundamentals

Pointer C का सबसे important और initially confusing topic है। Pointer एक variable है जो किसी दूसरे variable का memory address store करता है।

## Address क्या होता है?

हर variable memory में कहीं store होता है। उस location को address कहते हैं।

```c
int number = 42;
printf("%p\n", (void *)&number);
```

`&number` variable का address देता है।

## Pointer Declaration

```c
int number = 42;
int *ptr = &number;
```

यहां `ptr` एक pointer है जो `int` variable का address store करता है।

## Dereferencing

Pointer के address पर stored value access करने को dereferencing कहते हैं।

```c
printf("%d\n", *ptr);
```

## Value Change करना

```c
*ptr = 100;
printf("%d\n", number); // 100
```

Pointer से original variable की value change हो सकती है।

## Why Pointers Matter?

Pointers का use होता है:

- Functions से values modify करने में
- Arrays और strings के साथ
- Dynamic memory allocation में
- Data structures जैसे linked list में

## Practice

1. एक `int` variable बनाकर उसका address print करें।
2. Pointer से variable की value change करें।
3. Function में pointer pass करके दो numbers swap करें।
