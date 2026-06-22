# Chapter 7 — Loops और Iteration

Loop का use same काम बार-बार करने के लिए होता है। इससे repeated code लिखने की जरूरत नहीं पड़ती।

## `for` Loop

जब repetition count पहले से पता हो, `for` loop अच्छा होता है।

```c
for (int i = 1; i <= 5; i++) {
    printf("%d\n", i);
}
```

## `while` Loop

जब condition के आधार पर loop चलाना हो:

```c
int i = 1;
while (i <= 5) {
    printf("%d\n", i);
    i++;
}
```

## `do while` Loop

`do while` कम से कम एक बार जरूर चलता है।

```c
int choice;
do {
    printf("Enter 0 to exit: ");
    scanf("%d", &choice);
} while (choice != 0);
```

## `break` और `continue`

- `break` loop को तुरंत stop करता है।
- `continue` current iteration skip करके next iteration पर जाता है।

## Practice

1. 1 से 100 तक numbers print करें।
2. किसी number की multiplication table print करें।
3. User से numbers लेते रहें जब तक user 0 enter न करे।
