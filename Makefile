CC := gcc

SRC := $(wildcard *.c)
OBJ := $(SRC:.c=.o)

PROG = a.out

%.o: %.c %.h
	$(CC) -c $< -o $@

$(PROG): $(OBJ)
	$(CC) $^

clean:
	rm *.o $(PROG)
