# C Programming Examples

This folder contains small C programs arranged from beginner topics to slightly more advanced topics. Each file is meant to be compiled and run separately.

## Compile one file

```sh
gcc 01_hello_world.c -o 01_hello_world
./01_hello_world
```

## Compile with warnings

```sh
gcc -Wall -Wextra -std=c11 08_arrays.c -o 08_arrays
./08_arrays
```

## Compile all examples

```sh
make
```

## Remove compiled programs

```sh
make clean
```

## Suggested Order

1. `01_hello_world.c` - first C program
2. `02_variables.c` - common data types
3. `03_input_output.c` - reading input with `scanf`
4. `04_operators.c` - arithmetic and comparisons
5. `05_if_else.c` - decisions
6. `06_switch.c` - menu-style decisions
7. `07_loops.c` - `for`, `while`, and `do while`
8. `08_arrays.c` - storing many values
9. `09_strings.c` - character arrays and strings
10. `10_functions.c` - reusable blocks of code
11. `11_pointers.c` - addresses and pointer values
12. `12_structs.c` - grouping related data
13. `13_file_io.c` - writing and reading files
14. `14_dynamic_memory.c` - memory allocation with `malloc`
15. `15_recursion.c` - functions calling themselves

