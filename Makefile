CC := gcc

STATIC := main.c hjkl.c
DYNA := asdf.c
OBJ_STATIC += $(STATIC:.c=.o)
OBJ_DYNA += $(DYNA:.c=.o)

LDFLAGS = -L. $(DYNA:%.c=-l%)

PROG = a.out

all: libasdf.so $(PROG)

hjkl.o: hjkl.c hjkl.h
	$(CC) -c $< -o $@

asdf.o: asdf.c asdf.h
	$(CC) -c -fPIC $< -o $@

libasdf.so: asdf.o
	$(CC) $< -shared -o $@

$(PROG): $(OBJ_STATIC)
	$(CC) $^ $(LDFLAGS)

clean:
	rm *.o $(PROG)
