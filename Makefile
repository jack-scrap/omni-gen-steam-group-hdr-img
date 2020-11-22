CC = gcc

SRC = $(wildcard *.c)
OBJ = $(SRC:.c=.o)

a.out: $(OBJ)
	$(CC) $^ -o $@
