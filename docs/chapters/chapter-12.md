# Chapter 12 — Structures, Unions, and Enumerations

## Introduction

Every type you have used so far — `int`, `double`, `char`, arrays, strings — describes a single kind of value or a uniform sequence of one kind of value. Real-world data is rarely that tidy. A student record has a name (text), a roll number (an integer), and a GPA (a floating-point number) — three different types that belong together as one logical unit. This chapter introduces three tools for building exactly these kinds of custom, composite types: **structures** (`struct`), **unions** (`union`), and **enumerations** (`enum`).

## Structures: Grouping Related Data

A `struct` lets you bundle several variables — possibly of different types — under a single name, so they can be declared, passed, and referred to as one unit.

### Declaring a Struct and Creating Variables

```c
#include <stdio.h>

struct Point {
    int x;
    int y;
};

int main(void) {
    struct Point p1;
    p1.x = 3;
    p1.y = 7;

    printf("p1 = (%d, %d)\n", p1.x, p1.y);

    return 0;
}
```

### Compile and run

```bash
gcc -std=c11 -Wall -Wextra point.c -o point
./point
```

### Expected output

```text
p1 = (3, 7)
```

`struct Point` declares a new **type** named `Point` (tagged with `struct`), listing its **members**: `x` and `y`, both `int`. This declaration alone allocates no memory — it is a blueprint. The line `struct Point p1;` is what actually creates a variable, reserving enough memory to hold both members. The dot operator `.` accesses or sets an individual member, as seen with `p1.x` and `p1.y`.

You can also initialize a struct variable at the point of declaration, member by member in order, similar to array initialization:

```c
#include <stdio.h>

struct Point {
    int x;
    int y;
};

int main(void) {
    struct Point origin = {0, 0};
    struct Point p2 = {5, -2};

    printf("origin = (%d, %d)\n", origin.x, origin.y);
    printf("p2 = (%d, %d)\n", p2.x, p2.y);

    return 0;
}
```

### Compile and run

```bash
gcc -std=c11 -Wall -Wextra point2.c -o point2
./point2
```

### Expected output

```text
origin = (0, 0)
p2 = (5, -2)
```

You can also use **designated initializers**, naming each member explicitly — useful for clarity or when skipping members (unnamed members default to zero):

```c
struct Point p3 = {.x = 10, .y = 20};
```

### The `typedef struct { ... } Name;` Pattern

Writing `struct Point` every time you want to mention the type is repetitive. C provides `typedef` to create an alias for a type, and combining it with a struct declaration is extremely common.

There are two common styles. The first keeps the tag (`Point`) and adds an alias:

```c
typedef struct Point {
    int x;
    int y;
} Point;
```

After this, you may write either `struct Point p;` or simply `Point p;` — both refer to the same type.

The second style skips the tag entirely, naming only the typedef:

```c
typedef struct {
    int x;
    int y;
} Point;
```

Here there is no `struct Point` name at all — `Point` is the *only* way to refer to the type. This is the form you will see most often in modern C code, because in most programs nothing needs the tag name directly. The full example, restated with this idiom:

```c
#include <stdio.h>

typedef struct {
    int x;
    int y;
} Point;

int main(void) {
    Point p1 = {3, 7};
    printf("p1 = (%d, %d)\n", p1.x, p1.y);
    return 0;
}
```

### Compile and run

```bash
gcc -std=c11 -Wall -Wextra point3.c -o point3
./point3
```

### Expected output

```text
p1 = (3, 7)
```

!!! note "Why bother with typedef?"
    `typedef` does not create a new type from scratch — it only gives an existing type a new name. The benefit is purely readability and reduced typing: `Point` instead of `struct Point`, everywhere the type is used. For larger structs used throughout a program, this adds up.

### Nested Structs

A struct member can itself be another struct, letting you build hierarchical data:

```c
#include <stdio.h>

typedef struct {
    int x;
    int y;
} Point;

typedef struct {
    Point top_left;
    Point bottom_right;
} Rectangle;

int main(void) {
    Rectangle r;
    r.top_left.x = 0;
    r.top_left.y = 0;
    r.bottom_right.x = 10;
    r.bottom_right.y = 5;

    int width = r.bottom_right.x - r.top_left.x;
    int height = r.bottom_right.y - r.top_left.y;

    printf("Rectangle: width = %d, height = %d\n", width, height);

    return 0;
}
```

### Compile and run

```bash
gcc -std=c11 -Wall -Wextra rectangle.c -o rectangle
./rectangle
```

### Expected output

```text
Rectangle: width = 10, height = 5
```

Accessing a nested member chains the dot operator: `r.top_left.x` means "the `x` member of the `top_left` member of `r`."

## Structs and Functions

### Passing a Struct by Value

Recall from Chapter 8 that function arguments in C are passed by value — the function receives a *copy*. This applies to structs exactly as it applies to `int` or `double`: passing a struct to a function copies every one of its members.

```c
#include <stdio.h>

typedef struct {
    int x;
    int y;
} Point;

void print_point(Point p) {
    printf("(%d, %d)\n", p.x, p.y);
}

void try_to_move(Point p) {
    p.x += 100;  /* modifies only the local copy */
}

int main(void) {
    Point a = {1, 2};

    try_to_move(a);
    print_point(a);  /* unchanged: pass-by-value copied 'a' */

    return 0;
}
```

### Compile and run

```bash
gcc -std=c11 -Wall -Wextra structval.c -o structval
./structval
```

### Expected output

```text
(1, 2)
```

For a small struct like `Point` (two `int`s), copying is cheap. But for a struct with many members, or large arrays embedded inside it, copying the entire struct on every function call wastes time and stack space — and any modification the function makes is lost when the copy disappears at the end of the call.

### The Idiomatic Fix: Passing a Pointer to a Struct

Recall from Chapter 11 that a pointer stores the *address* of a variable, and that passing a pointer to a function lets the function read or modify the original variable without copying it. The same technique works for structs:

```c
#include <stdio.h>

typedef struct {
    int x;
    int y;
} Point;

void move(Point *p, int dx, int dy) {
    (*p).x += dx;
    (*p).y += dy;
}

int main(void) {
    Point a = {1, 2};

    move(&a, 10, 20);
    printf("(%d, %d)\n", a.x, a.y);

    return 0;
}
```

### Compile and run

```bash
gcc -std=c11 -Wall -Wextra structptr.c -o structptr
./structptr
```

### Expected output

```text
(11, 22)
```

`move` receives `&a` — the address of `a` — into a parameter `Point *p`. Inside the function, `*p` dereferences the pointer to get back the original struct, and `(*p).x` accesses its `x` member. Because no copy was made, the modification is visible in `main` after the call returns. This is both faster (only an address is copied, regardless of struct size) and necessary whenever the function needs to modify the caller's struct.

### The Arrow Operator `->`

Writing `(*p).x` is correct but visually noisy — the parentheses are required because `.` binds tighter than `*`, so `*p.x` (without parentheses) would incorrectly try to access a member `x` of `p` first, which fails to compile since `p` is a pointer, not a struct. C provides the **arrow operator** `->` as shorthand: `p->x` means exactly `(*p).x`.

```c
#include <stdio.h>

typedef struct {
    int x;
    int y;
} Point;

void move(Point *p, int dx, int dy) {
    p->x += dx;
    p->y += dy;
}

void print_point(const Point *p) {
    printf("(%d, %d)\n", p->x, p->y);
}

int main(void) {
    Point a = {1, 2};

    move(&a, 10, 20);
    print_point(&a);

    return 0;
}
```

### Compile and run

```bash
gcc -std=c11 -Wall -Wextra arrow.c -o arrow
./arrow
```

### Expected output

```text
(11, 22)
```

!!! tip "const for read-only struct pointers"
    `print_point` takes a `const Point *p` — a pointer to a struct the function promises not to modify. This combines the performance benefit of passing a pointer (no copy) with the safety of pass-by-value (the caller's data cannot be changed), and is idiomatic whenever a function only needs to read struct data.

## Arrays of Structs

A struct is a type like any other, so you can declare an array whose elements are all structs of the same type — the natural representation of a table of records.

```c
#include <stdio.h>

typedef struct {
    char name[20];
    int roll_number;
    double gpa;
} Student;

int main(void) {
    Student class_list[3] = {
        {"Asha",   101, 8.7},
        {"Brijesh", 102, 7.9},
        {"Chitra", 103, 9.1}
    };

    for (int i = 0; i < 3; i++) {
        printf("Roll %d: %-8s GPA %.1f\n",
               class_list[i].roll_number,
               class_list[i].name,
               class_list[i].gpa);
    }

    return 0;
}
```

### Compile and run

```bash
gcc -std=c11 -Wall -Wextra students.c -o students
./students
```

### Expected output

```text
Roll 101: Asha     GPA 8.7
Roll 102: Brijesh  GPA 7.9
Roll 103: Chitra   GPA 9.1
```

Each element `class_list[i]` is a complete `Student` struct, and `class_list[i].name` accesses its `name` member, recalling from Chapter 9 and Chapter 10 that array indexing and struct member access combine naturally. Passing this array to a function works exactly as with any array (Chapter 9): the array decays to a pointer to its first element, so iterating over it inside a function is cheap regardless of how many `Student` records it holds.

```c
#include <stdio.h>

typedef struct {
    char name[20];
    int roll_number;
    double gpa;
} Student;

void print_roster(const Student roster[], int count) {
    for (int i = 0; i < count; i++) {
        printf("Roll %d: %-8s GPA %.1f\n",
               roster[i].roll_number, roster[i].name, roster[i].gpa);
    }
}

int main(void) {
    Student class_list[3] = {
        {"Asha",    101, 8.7},
        {"Brijesh", 102, 7.9},
        {"Chitra",  103, 9.1}
    };

    print_roster(class_list, 3);

    return 0;
}
```

### Compile and run

```bash
gcc -std=c11 -Wall -Wextra roster.c -o roster
./roster
```

### Expected output

```text
Roll 101: Asha     GPA 8.7
Roll 102: Brijesh  GPA 7.9
Roll 103: Chitra   GPA 9.1
```

## Unions: Sharing Memory Between Members

A `union` looks syntactically like a struct, but its members **share the same memory** rather than each having their own separate storage. Only one member is meaningful at any given time — writing to one member overwrites the bytes that any other member would read.

```c
#include <stdio.h>

union Value {
    int i;
    float f;
    char c;
};

int main(void) {
    union Value v;

    v.i = 65;
    printf("As int: %d\n", v.i);

    v.f = 3.14f;
    printf("As float: %.2f\n", v.f);
    /* v.i is no longer meaningful here — the bytes now hold a float */

    return 0;
}
```

### Compile and run

```bash
gcc -std=c11 -Wall -Wextra unionbasic.c -o unionbasic
./unionbasic
```

### Expected output

```text
As int: 65
As float: 3.14
```

### `sizeof` a Union vs. an Equivalent Struct

A struct's size is (at least) the sum of its members' sizes, since each member gets its own space. A union's size is only as large as its *largest* member, since all members overlap in the same space.

```c
#include <stdio.h>

struct AsStruct {
    int i;
    float f;
    char c;
};

union AsUnion {
    int i;
    float f;
    char c;
};

int main(void) {
    printf("sizeof(struct AsStruct) = %zu\n", sizeof(struct AsStruct));
    printf("sizeof(union AsUnion)   = %zu\n", sizeof(union AsUnion));

    return 0;
}
```

### Compile and run

```bash
gcc -std=c11 -Wall -Wextra sizecompare.c -o sizecompare
./sizecompare
```

### Expected output

```text
sizeof(struct AsStruct) = 12
sizeof(union AsUnion)   = 4
```

!!! note "Padding may affect exact numbers"
    The struct's exact size can be larger than the simple sum of its members due to **padding** the compiler inserts for alignment purposes; the exact byte counts can vary slightly by platform and compiler. The key relationship to remember is: a struct's size grows with each member added, while a union's size is fixed at its single largest member, regardless of how many members it has.

### A Realistic Use Case: Tagged Unions

Used alone, a union has a serious problem: nothing about it tells you *which* member was last written, so reading the wrong member produces meaningless data. The standard fix is to pair the union with a separate field — often an `enum` — that records which member is currently valid. This pairing is called a **tagged union**.

```c
#include <stdio.h>

typedef enum {
    TYPE_INT,
    TYPE_FLOAT,
    TYPE_CHAR
} ValueType;

typedef struct {
    ValueType type;
    union {
        int i;
        float f;
        char c;
    } data;
} TaggedValue;

void print_value(TaggedValue v) {
    switch (v.type) {
        case TYPE_INT:
            printf("int: %d\n", v.data.i);
            break;
        case TYPE_FLOAT:
            printf("float: %.2f\n", v.data.f);
            break;
        case TYPE_CHAR:
            printf("char: %c\n", v.data.c);
            break;
    }
}

int main(void) {
    TaggedValue a = {TYPE_INT, {.i = 42}};
    TaggedValue b = {TYPE_FLOAT, {.f = 2.5f}};
    TaggedValue c = {TYPE_CHAR, {.c = 'Z'}};

    print_value(a);
    print_value(b);
    print_value(c);

    return 0;
}
```

### Compile and run

```bash
gcc -std=c11 -Wall -Wextra taggedunion.c -o taggedunion
./taggedunion
```

### Expected output

```text
int: 42
float: 2.50
char: Z
```

Here, `TaggedValue` stores both a `ValueType type` discriminant and the `union` payload. Code that receives a `TaggedValue` checks `v.type` first to know which union member is safe to read — exactly the pattern this example's `switch` (recall Chapter 6) demonstrates.

!!! warning "Reading the wrong union member"
    Reading a union member other than the one most recently written is not generally meaningful — the bytes will be reinterpreted according to the new member's type, which usually produces nonsensical values rather than a clean error. There is no automatic check; the discipline of tracking which member is valid (as with the tag above) is entirely the programmer's responsibility.

## Enumerations: Named Integer Constants

An `enum` declares a set of named integer constants, making code that represents a fixed set of options far more readable than using raw numbers ("magic numbers").

```c
#include <stdio.h>

enum Day {
    MONDAY,
    TUESDAY,
    WEDNESDAY,
    THURSDAY,
    FRIDAY,
    SATURDAY,
    SUNDAY
};

int main(void) {
    enum Day today = WEDNESDAY;

    printf("Day number: %d\n", today);

    if (today == SATURDAY || today == SUNDAY) {
        printf("It's the weekend.\n");
    } else {
        printf("It's a weekday.\n");
    }

    return 0;
}
```

### Compile and run

```bash
gcc -std=c11 -Wall -Wextra day.c -o day
./day
```

### Expected output

```text
Day number: 2
It's a weekday.
```

By default, enum constants are assigned increasing `int` values starting at 0: `MONDAY` is 0, `TUESDAY` is 1, and so on, so `WEDNESDAY` prints as `2`. The names exist purely for the programmer's benefit — at compile time, `today == SATURDAY` is really just an integer comparison.

You can override the default numbering by assigning explicit values; any constant not given a value continues counting up from the previous one.

```c
#include <stdio.h>

enum Color {
    RED = 1,
    GREEN = 2,
    BLUE = 4,
    YELLOW = 8
};

int main(void) {
    enum Color favorite = BLUE;

    printf("favorite color code: %d\n", favorite);

    return 0;
}
```

### Compile and run

```bash
gcc -std=c11 -Wall -Wextra color.c -o color
./color
```

### Expected output

```text
favorite color code: 4
```

Assigning powers of two (1, 2, 4, 8, ...) like this is a common pattern when the values may later be combined using bitwise operators (recall Chapter 4) as flags — though that combination technique is not pursued further here.

As with structs, `enum` is usually paired with `typedef` so the type can be named without repeating the `enum` keyword:

```c
typedef enum {
    RED,
    GREEN,
    BLUE
} Color;
```

## A Brief Aside: `typedef` Beyond Structs

`typedef` is a general tool for naming any existing type, not only struct/union/enum types. A common use is shortening verbose built-in type names:

```c
#include <stdio.h>

typedef unsigned long ulong;

int main(void) {
    ulong distance = 4000000000UL;
    printf("distance = %lu\n", distance);
    return 0;
}
```

### Compile and run

```bash
gcc -std=c11 -Wall -Wextra ulong.c -o ulong
./ulong
```

### Expected output

```text
distance = 4000000000
```

`ulong` is now simply another name for `unsigned long`; the compiler treats them identically. Used sparingly and consistently, this can improve readability, especially for long or frequently repeated type names.

## Summary

- `struct` groups related fields, possibly of different types, under one named type; members are accessed and set with the dot operator (`.`).
- `typedef struct { ... } Name;` gives a struct type a short alias, avoiding the need to write `struct Tag` everywhere; the tagless form (typedef only, no struct tag) is the most common modern style.
- Structs can be nested: a struct member can itself be a struct, accessed by chaining dot operators.
- Passing a struct by value copies all of its members, which is correct for small structs but costly for large ones and cannot modify the caller's data; passing a pointer to the struct (`struct Point *p`) avoids the copy and allows modification.
- The arrow operator `p->member` is shorthand for `(*p).member`, used whenever you have a pointer to a struct.
- Arrays of structs represent tables of records; each element is indexed and accessed like any array element, with member access chained on top (`array[i].member`).
- A `union` makes all of its members share the same memory; only the most recently written member holds meaningful data, and `sizeof(union)` equals the size of its largest member, in contrast to a struct whose size grows with every member.
- A tagged union pairs a union with a separate discriminant field (often an `enum`) recording which member is currently valid — a realistic, safe way to use unions.
- An `enum` declares named integer constants, defaulting to 0, 1, 2, ... in declaration order, or to explicit values you assign yourself.
- `typedef` is a general-purpose tool for naming any type, not just structs — e.g., `typedef unsigned long ulong;`.

## Exercises

1. Define a `struct` named `Time` with `int` members `hours`, `minutes`, and `seconds`. Write a program that creates a `Time` variable initialized to a value of your choosing and prints it as `HH:MM:SS` using appropriate zero-padding with `printf` field width/flags (recall Chapter 5).
2. Using the tagless `typedef struct { ... } Name;` pattern, define a `Book` type with `title` (a `char` array), `author` (a `char` array), and `year` (an `int`). Create an array of 4 `Book` values and print them all in a loop.
3. Write a function `void scale(Point *p, int factor)` that multiplies both members of a `Point` by `factor`, using the arrow operator. Call it from `main` and print the result before and after to confirm it modified the original.
4. Define a `union Number` with `int as_int`, `float as_float`, and `double as_double` members. Print `sizeof` of the union and of an equivalent struct with the same three members, and explain in a comment why the two sizes differ the way they do.
5. *(Harder)* Build a small tagged-union "shape" type: an `enum ShapeType { CIRCLE, RECTANGLE }`, and a struct containing that enum plus a union holding either a `radius` (for circles) or `width`/`height` (for rectangles, as a nested struct inside the union). Write a function `double area(ShapeType_struct s)` (choose your own type name) that uses a `switch` on the tag to compute the correct area for either shape, and test it with one circle and one rectangle.
6. *(Harder)* Write a program that defines `enum Direction { NORTH, EAST, SOUTH, WEST }` and a function `enum Direction turn_right(enum Direction d)` that returns the direction 90 degrees clockwise from `d` (so `NORTH` becomes `EAST`, `WEST` becomes `NORTH`, and so on), using the fact that the enum constants are really just integers 0–3 and modular arithmetic (recall the `%` operator from Chapter 4). Test it by starting at each of the four directions and turning right four times, confirming you arrive back where you started.

---
[← Chapter 11 — Pointers - Fundamentals](chapter-11.md) | [Chapter 13 — Dynamic Memory Allocation →](chapter-13.md)
