CC=gcc
CFLAGS=-Wall -O2 -march=native
LFLAGS=-lprocps

all: main

main: main.c
	$(CC) $(CFLAGS) $< -o $@ $(LFLAGS)

clean:
	rm -Rf *~ *.o main
