CXX := g++

STATIC := main.cpp hjkl.cpp
DYNA := asdf.cpp
OBJ_STATIC += $(STATIC:.cpp=.o)
OBJ_DYNA += $(DYNA:.cpp=.o)

LDFLAGS = -L. $(DYNA:%.cpp=-l%)

PROG = a.out

all: libasdf.so $(PROG)

asdf.o: asdf.cpp asdf.h
	$(CXX) -c -fPIC $< -o $@

libasdf.so: asdf.o
	$(CXX) $< -shared -o $@

$(PROG): $(OBJ_STATIC)
	$(CXX) $^ $(LDFLAGS)

clean:
	rm *.o $(PROG)
