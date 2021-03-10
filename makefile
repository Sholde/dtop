# Variable
## Compilation
CC=gcc
CFLAGS=-g3 -Wall
OFLAGS=-O2 -march=native
LFLAGS=-lprocps -lpthread

## Name
PROG=dtop

# Target
all: $(PROG)

$(PROG): main.o sensor.o display.o server.o client.o io.o
	$(CC) $(CFLAGS) $(OFLAGS) $^ -o $@ $(LFLAGS)

main.o: main.c sensor.c sensor.h display.c display.h io.h struct.h

sensor.o: sensor.c sensor.h struct.h

display.o: display.c display.h struct.h

server.o: server.c server.h io.c io.h

client.o: client.c client.h server.h io.c io.h

io.o: io.c io.h

%.o: %.c
	$(CC) $(CFLAGS) $(OFLAGS) -c $< -o $@ $(LFLAGS)

clean:
	rm -Rf *~ *.o $(PROG)
