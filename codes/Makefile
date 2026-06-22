CC = gcc
CFLAGS = -Wall -Wextra -std=c11

SOURCES = $(wildcard [0-9][0-9]_*.c)
PROGRAMS = $(SOURCES:.c=)

.PHONY: all clean

all: $(PROGRAMS)

%: %.c
	$(CC) $(CFLAGS) $< -o $@

clean:
	rm -f $(PROGRAMS) student.txt

