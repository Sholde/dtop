CC=gcc

all: main

main: main.c
	$(CC) $< -o $@

clean:
	rm -Rf *~ *.o main
