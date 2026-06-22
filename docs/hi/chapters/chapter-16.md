# Chapter 16 — Storage Classes, Scope और Recursion

इस chapter में हम variable lifetime, visibility और recursive functions समझेंगे।

## Scope

Scope बताता है कि variable code के किस part में accessible है।

```c
int global_value = 10;

int main(void) {
    int local_value = 20;
    return 0;
}
```

`global_value` पूरे file में accessible है। `local_value` सिर्फ `main` के अंदर accessible है।

## Storage Classes

| Keyword | Meaning |
| --- | --- |
| `auto` | default local variable |
| `static` | value function calls के बीच बची रहती है |
| `extern` | variable किसी दूसरी file में defined है |
| `register` | compiler hint, rarely needed |

## Static Example

```c
void counter(void) {
    static int count = 0;
    count++;
    printf("%d\n", count);
}
```

`count` function खत्म होने के बाद भी value retain करता है।

## Recursion

जब function खुद को call करता है, उसे recursion कहते हैं।

```c
int factorial(int n) {
    if (n <= 1) {
        return 1;
    }
    return n * factorial(n - 1);
}
```

Base case जरूरी है, नहीं तो infinite recursion हो सकता है।

## Practice

1. Static counter function बनाएं।
2. Recursive factorial लिखें।
3. Recursive Fibonacci लिखें।
