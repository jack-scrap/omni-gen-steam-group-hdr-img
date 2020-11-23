CC := gcc

SRC := $(wildcard *.c)
OBJ := $(SRC:.c=.o)

%.o: %.c %.h
	$(CC) -c $< -o $@

a.out: $(OBJ)
	$(CC) $^

clean:
	rm *.o a.out
