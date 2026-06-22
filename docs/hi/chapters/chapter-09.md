# Chapter 9 — Arrays

Array same type की multiple values store करने के लिए use होता है।

## Array Declaration

```c
int marks[5];
```

यह 5 integers के लिए space बनाता है। Indexing 0 से start होती है।

```c
marks[0] = 90;
marks[1] = 85;
```

## Array Initialization

```c
int numbers[5] = {10, 20, 30, 40, 50};
```

## Loop के साथ Array

```c
for (int i = 0; i < 5; i++) {
    printf("%d\n", numbers[i]);
}
```

## Sum Example

```c
int sum = 0;
for (int i = 0; i < 5; i++) {
    sum += numbers[i];
}
printf("Sum = %d\n", sum);
```

## Common Mistake

Array bounds से बाहर access करना dangerous है।

```c
int a[3] = {1, 2, 3};
printf("%d", a[5]); // गलत
```

## Practice

1. 5 marks input लेकर average निकालें।
2. Array में largest number find करें।
3. Array को reverse order में print करें।
