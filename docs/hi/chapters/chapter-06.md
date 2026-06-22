# Chapter 6 — Decision Making: `if`, `else`, `switch`

Decision making से program अलग-अलग conditions के हिसाब से अलग काम करता है।

## `if` Statement

```c
if (marks >= 50) {
    printf("Pass\n");
}
```

Condition true हो तो block execute होता है।

## `if else`

```c
if (marks >= 50) {
    printf("Pass\n");
} else {
    printf("Fail\n");
}
```

## `else if` Ladder

```c
if (marks >= 90) {
    printf("Grade A\n");
} else if (marks >= 75) {
    printf("Grade B\n");
} else if (marks >= 50) {
    printf("Grade C\n");
} else {
    printf("Fail\n");
}
```

## `switch` Statement

जब एक variable की fixed values पर decision लेना हो, `switch` readable होता है।

```c
switch (choice) {
case 1:
    printf("Add\n");
    break;
case 2:
    printf("Subtract\n");
    break;
default:
    printf("Invalid choice\n");
}
```

`break` important है। इसके बिना execution next case में गिर सकता है।

## Practice

1. Number positive, negative या zero है यह check करें।
2. Marks से grade calculate करें।
3. Calculator menu बनाएं: add, subtract, multiply, divide।
