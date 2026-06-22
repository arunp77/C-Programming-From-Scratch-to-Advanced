# Chapter 12 — Structures, Unions और Enums

C में `struct` related data को एक group में रखने के लिए use होता है।

## Structure

```c
struct Student {
    int roll_no;
    char name[50];
    float marks;
};
```

यह एक custom data type जैसा काम करता है।

## Structure Variable

```c
struct Student s1;
s1.roll_no = 1;
s1.marks = 95.5f;
```

Dot operator `.` से members access करते हैं।

## Example

```c
struct Student s = {1, "Asha", 91.5f};
printf("%s scored %.2f\n", s.name, s.marks);
```

## Union

`union` में members same memory share करते हैं। एक समय में generally एक member meaningful होता है।

```c
union Data {
    int i;
    float f;
};
```

## Enum

`enum` named integer constants देता है।

```c
enum Day { MON, TUE, WED, THU, FRI, SAT, SUN };
```

## Practice

1. `Book` structure बनाएं: title, author, price।
2. 3 students का array बनाकर marks print करें।
3. Weekdays के लिए enum use करें।
