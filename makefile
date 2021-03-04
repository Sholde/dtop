# Variable
## Compilation
CC=gcc
CFLAGS=-Wall -O2 -march=native
LFLAGS=-lprocps -lpthread

## Name
PROG=dtop

# Target
all: $(PROG)

$(PROG): main.o sensor.o display.o server.o client.o
	$(CC) $(CFLAGS) $^ -o $@ $(LFLAGS)

main.o: main.c sensor.c sensor.h display.c display.h struct.h

sensor.o: sensor.c sensor.h struct.h

display.o: display.c display.h struct.h

server.o: server.c server.h

client.o: client.c client.h server.h

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@ $(LFLAGS)

clean:
	rm -Rf *~ *.o $(PROG)
