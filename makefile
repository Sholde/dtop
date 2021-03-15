# Variable
## Compilation
CC=gcc
CFLAGS=-g3 -Wall
OFLAGS=-O2 -march=native
LFLAGS=-lprocps -lpthread -lncurses

## Name
PROG=dtop
SRC_DIR=src
OBJ_DIR=obj

.PHONY: all prepare clean generate_man
# Target
all: prepare $(PROG)

prepare:
	mkdir -p $(OBJ_DIR)

$(PROG): $(OBJ_DIR)/main.o $(OBJ_DIR)/sensor.o $(OBJ_DIR)/display.o            \
	 $(OBJ_DIR)/server.o $(OBJ_DIR)/client.o $(OBJ_DIR)/io.o
	$(CC) $(CFLAGS) $(OFLAGS) $^ -o $@ $(LFLAGS)

$(OBJ_DIR)/main.o: $(SRC_DIR)/main.c $(SRC_DIR)/sensor.c $(SRC_DIR)/sensor.h   \
		   $(SRC_DIR)/display.c $(SRC_DIR)/display.h $(SRC_DIR)/io.h   \
		   $(SRC_DIR)/struct.h

$(OBJ_DIR)/sensor.o: $(SRC_DIR)/sensor.c $(SRC_DIR)/sensor.h $(SRC_DIR)/struct.h

$(OBJ_DIR)/display.o: $(SRC_DIR)/display.c $(SRC_DIR)/display.h                \
		      $(SRC_DIR)/struct.h

$(OBJ_DIR)/server.o: $(SRC_DIR)/server.c $(SRC_DIR)/server.h $(SRC_DIR)/io.c   \
		     $(SRC_DIR)/io.h $(SRC_DIR)/struct.h

$(OBJ_DIR)/client.o: $(SRC_DIR)/client.c $(SRC_DIR)/client.h                   \
		     $(SRC_DIR)/server.h $(SRC_DIR)/io.c $(SRC_DIR)/io.h       \
		     $(SRC_DIR)/struct.h

$(OBJ_DIR)/io.o: $(SRC_DIR)/io.c $(SRC_DIR)/io.h

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	$(CC) $(CFLAGS) $(OFLAGS) -c $< -o $@ $(LFLAGS)

generate_man: 
	pandoc $(PROG).md -s -t man > $(PROG).1

clean:
	rm -Rf *~ *.o $(PROG) $(OBJ_DIR)
