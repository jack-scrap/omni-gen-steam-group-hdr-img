CC = gcc

SRC = main.c
OBJ = $(SRC:.c=.o)

a.out: $(OBJ)
	$(CC) $^ -o $@
