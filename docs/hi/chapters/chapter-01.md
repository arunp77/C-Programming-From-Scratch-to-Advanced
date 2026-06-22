# Chapter 1 — Environment Setup

इस chapter में आप C program compile और run करने के लिए basic setup करेंगे।

## आपको क्या चाहिए?

1. एक text editor, जैसे VS Code
2. एक C compiler, जैसे `gcc`
3. एक terminal, जहां आप commands run करेंगे

## Ubuntu / Debian / WSL

```bash
sudo apt update
sudo apt install build-essential gdb
gcc --version
```

अगर `gcc --version` output दिखाता है, तो compiler install हो गया है।

## पहला compile command

मान लीजिए file का नाम `hello.c` है:

```bash
gcc hello.c -o hello
./hello
```

इसमें:

- `gcc hello.c` source code compile करता है।
- `-o hello` output program का नाम `hello` रखता है।
- `./hello` compiled program run करता है।

## Warning flags use करें

Learning के समय warnings बहुत helpful होती हैं:

```bash
gcc -Wall -Wextra -std=c11 hello.c -o hello
```

## Common Problems

| Problem | Meaning | Fix |
| --- | --- | --- |
| `gcc: command not found` | Compiler install नहीं है | `build-essential` install करें |
| `No such file or directory` | File name या path गलत है | सही folder में जाएं |
| `Permission denied` | Program executable नहीं है | compile command दोबारा run करें |

Next: [Chapter 2 — पहला C Program](chapter-02.md)
