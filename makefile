# Variable
## Compilation
CC=gcc
CFLAGS=-Wall -O2 -march=native
LFLAGS=-lprocps

## Name
PROG=dtop

# Target
all: $(PROG)

$(PROG): main.o sensor.o display.o
	$(CC) $(CFLAGS) $^ -o $@ $(LFLAGS)

main.o: main.c sensor.c sensor.h display.c display.h struct.h

sensor.o: sensor.c sensor.h struct.h

display.o: display.c display.h struct.h

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@ $(LFLAGS)

clean:
	rm -Rf *~ *.o $(PROG)
