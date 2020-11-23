CXX := g++

STATIC := main.cpp hjkl.cpp
DYNA := asdf.cpp
OBJ_STATIC += $(STATIC:.c=.o)
OBJ_DYNA += $(DYNA:.c=.o)

LDFLAGS = -L. $(DYNA:%.c=-l%)

PROG = a.out

all: libasdf.so $(PROG)

hjkl.o: hjkl.cpp hjkl.h
	$(CXX) -c $< -o $@

asdf.o: asdf.cpp asdf.h
	$(CXX) -c -fPIC $< -o $@

libasdf.so: asdf.o
	$(CXX) $< -shared -o $@

$(PROG): $(OBJ_STATIC)
	$(CXX) $^ $(LDFLAGS)

clean:
	rm *.o $(PROG)
