# Chapter 13 — Dynamic Memory Allocation

Dynamic memory allocation से program runtime पर memory मांग सकता है। यह तब useful है जब data size पहले से पता न हो।

## Stack vs Heap

- Normal local variables stack पर बनते हैं।
- `malloc`, `calloc`, `realloc` से memory heap पर मिलती है।
- Heap memory को `free` करना programmer की responsibility है।

## `malloc`

```c
int *numbers = malloc(5 * sizeof(int));
```

`malloc` memory allocate करता है और pointer return करता है।

## Always Check NULL

```c
if (numbers == NULL) {
    printf("Memory allocation failed\n");
    return 1;
}
```

## `free`

```c
free(numbers);
```

Memory use करने के बाद release करें।

## Example

```c
int count;
scanf("%d", &count);

int *arr = malloc((size_t)count * sizeof(int));
if (arr == NULL) {
    return 1;
}

free(arr);
```

## Common Mistakes

- `free` भूलना memory leak बनाता है।
- `free` के बाद pointer use करना undefined behavior है।
- गलत size allocate करना dangerous है।

## Practice

1. User से `n` लेकर `n` integers allocate करें।
2. Dynamic array का sum calculate करें।
3. `calloc` और `malloc` का difference test करें।
