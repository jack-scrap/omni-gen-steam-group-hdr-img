CC := gcc

STATIC := main.c
DYNA := asdf.c
OBJ += $(STATIC:.c=.o)
OBJ += $(DYNA:.c=.o)

LDFLAGS = -L. $(DYNA:%.c=-l%)

PROG = a.out

all: libasdf.so $(PROG)

asdf.o: asdf.c asdf.h
	$(CC) -c -fPIC $< -o $@

libasdf.so: asdf.o
	$(CC) $< -shared -o $@

$(PROG): $(STATIC)
	$(CC) $^ $(LDFLAGS)

clean:
	rm *.o $(PROG)
