# Chapter 14 — File Handling

File handling से program data को disk पर save और read कर सकता है।

## File Pointer

```c
FILE *file;
```

`FILE` type file stream represent करता है।

## File Open करना

```c
file = fopen("data.txt", "w");
```

Common modes:

| Mode | Meaning |
| --- | --- |
| `"r"` | read |
| `"w"` | write, old content हटेगा |
| `"a"` | append |

## Check for NULL

```c
if (file == NULL) {
    printf("Could not open file\n");
    return 1;
}
```

## Write to File

```c
fprintf(file, "Hello file\n");
fclose(file);
```

## Read from File

```c
char line[100];
file = fopen("data.txt", "r");
while (fgets(line, sizeof line, file) != NULL) {
    printf("%s", line);
}
fclose(file);
```

## Practice

1. User का name और age file में save करें।
2. File से text read करके screen पर print करें।
3. Marks की list file में append करें।
