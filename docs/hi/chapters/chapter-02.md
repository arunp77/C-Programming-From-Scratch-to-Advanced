# Chapter 2 — पहला C Program

अब हम पहला C program लिखेंगे, compile करेंगे और run करेंगे।

## Code

```c
#include <stdio.h>

int main(void) {
    printf("Hello, World!\n");
    return 0;
}
```

इसे `hello.c` नाम से save करें।

## Compile और Run

```bash
gcc -Wall -Wextra -std=c11 hello.c -o hello
./hello
```

Expected output:

```text
Hello, World!
```

## Line by line समझें

### `#include <stdio.h>`

यह line C की standard input/output library include करती है। `printf` इसी library से आता है।

### `int main(void)`

`main` function program की entry point होती है। जब program run होता है, execution यहीं से शुरू होती है।

### `printf("Hello, World!\n");`

यह screen पर message print करता है। `\n` new line के लिए use होता है।

### `return 0;`

यह operating system को बताता है कि program successfully complete हुआ।

## Practice

1. Message बदलकर अपना नाम print करें।
2. दो `printf` lines लिखें।
3. जानबूझकर semicolon हटाएं और compiler error देखें।
