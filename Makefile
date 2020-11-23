CXX := g++

STATIC := main.cpp
DYNA := asdf.cpp hjkl.cpp
OBJ_STATIC += $(STATIC:.cpp=.o)
OBJ_DYNA += $(DYNA:.cpp=.o)

LDFLAGS = -L. $(DYNA:%.cpp=-l%)

PROG = a.out

all: libasdf.so libhjkl.so $(PROG)

asdf.o: asdf.cpp asdf.h
	$(CXX) -c -fPIC $< -o $@

libasdf.so: asdf.o
	$(CXX) $< -shared -o $@

hjkl.o: hjkl.cpp hjkl.h
	$(CXX) -c -fPIC $< -o $@

libhjkl.so: hjkl.o
	$(CXX) $< -shared -o $@ -L. -lasdf

$(PROG): $(OBJ_STATIC)
	$(CXX) $^ $(LDFLAGS)

clean:
	rm *.o $(PROG)
