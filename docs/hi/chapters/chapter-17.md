# Chapter 17 — Advanced Pointers

Advanced pointers में pointer arithmetic, pointers to pointers, function pointers और dynamic data structures की foundation आती है।

## Pointer Arithmetic

Array और pointer closely related हैं।

```c
int arr[3] = {10, 20, 30};
int *p = arr;

printf("%d\n", *p);       // 10
printf("%d\n", *(p + 1)); // 20
```

Pointer arithmetic data type size के हिसाब से move करती है।

## Pointer to Pointer

```c
int x = 10;
int *p = &x;
int **pp = &p;

printf("%d\n", **pp);
```

`pp` pointer का address store करता है।

## Function Pointer

```c
int add(int a, int b) {
    return a + b;
}

int (*operation)(int, int) = add;
printf("%d\n", operation(2, 3));
```

Function pointers callbacks और flexible designs में useful हैं।

## `const` with Pointers

```c
const int *p;      // value change नहीं कर सकते through p
int *const p2 = &x; // pointer address change नहीं कर सकते
```

## Practice

1. Pointer arithmetic से array print करें।
2. Pointer to pointer example बनाएं।
3. Function pointer से calculator operation choose करें।
