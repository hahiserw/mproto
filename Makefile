PROG=mproto
CC=gcc
CFLAGS=-Wall -Wextra -std=gnu99 -pedantic -O2

SRC=$(wildcard src/*.c)
OBJ=$(patsubst src%, obj%, $(SRC:.c=.o))
OBJ32=$(patsubst src%, obj32%, $(SRC:.c=.o))


all: $(PROG) #$(PROG)32


$(PROG): $(OBJ)
	$(CC) $(CFLAGS) $^ -o $@

$(PROG)32: $(OBJ32)
	$(CC) $(CFLAGS) -m32 $^ -o $@

32: $(PROG)32


obj/%.o: src/%.c src/%.h
	$(CC) $(CFLAGS) -c $< -o $@

obj32/%.o: src/%.c src/%.h
	$(CC) $(CFLAGS) -m32 -c $< -o $@


clean:
	rm -rf $(OBJ) $(OBJ32)
	rm -f $(PROG)
