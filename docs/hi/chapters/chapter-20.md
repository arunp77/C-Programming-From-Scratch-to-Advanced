# Chapter 20 — Debugging और Best Practices

Debugging का मतलब है program में errors ढूंढना और fix करना। Good habits से bugs कम होते हैं।

## Compiler Warnings

हमेशा warnings enable करें:

```bash
gcc -Wall -Wextra -std=c11 program.c -o program
```

Warnings ignore मत करें। अक्सर warning future bug का signal होती है।

## Print Debugging

Simple debugging के लिए `printf` use कर सकते हैं:

```c
printf("value = %d\n", value);
```

## `gdb` Debugger

Debug symbols के साथ compile करें:

```bash
gcc -g program.c -o program
gdb ./program
```

Useful commands:

| Command | Use |
| --- | --- |
| `break main` | breakpoint set करें |
| `run` | program start करें |
| `next` | next line चलाएं |
| `print x` | variable value देखें |
| `quit` | exit |

## Best Practices

- Small functions लिखें।
- Meaningful variable names use करें।
- Input validate करें।
- `malloc` के बाद `NULL` check करें।
- Allocated memory को `free` करें।
- Code compile warnings के साथ test करें।

## Practice

1. किसी program में intentional bug डालकर warning देखें।
2. `gdb` से line-by-line execution करें।
3. अपने पुराने examples को warnings के साथ recompile करें।
