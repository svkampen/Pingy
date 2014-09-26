CC=clang
CFLAGS=-std=c11
LDFLAGS=-lpng -lm

all:
	$(CC) -o pingy $(CFLAGS) $(LDFLAGS) pingy.c
