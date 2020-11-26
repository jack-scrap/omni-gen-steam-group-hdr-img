CXX := g++

STATIC := main.cpp disp.cpp util.cpp
DYNA := asdf.cpp hjkl.cpp
OBJ_STATIC += $(STATIC:.cpp=.o)
OBJ_DYNA += $(DYNA:.cpp=.o)

LIB = -L. $(DYNA:%.cpp=-l%)
SDL = -lSDL2 -lSDL2_ttf
GL = -lGLEW -lGL
PY = -L/usr/lib/python3.6/config-3.6m-x86_64-linux-gnu -L/usr/lib -lpython3.6m -pthread -ldl  -lutil -lm  -Xlinker -export-dynamic -Wl,-O1 -Wl,-Bsymbolic-functions -I/usr/include/python3.6m -I/usr/include/python3.6m  -g -fdebug-prefix-map=/build/python3.6-0aiVHW/python3.6-3.6.9=. -specs=/usr/share/dpkg/no-pie-compile.specs -fstack-protector -DNDEBUG -g -fwrapv -O3 -fPIC
LDFLAGS += $(LIB)
LDFLAGS += $(SDL)
LDFLAGS += $(GL)
LDFLAGS += $(PY)

PROG = a.out

all: libprog.so libasdf.so libhjkl.so $(PROG)

prog.o: prog.cpp prog.h
	$(CXX) -c -fPIC $< -o $@

libprog.so: prog.o
	$(CXX) $< -shared -o $@

asdf.o: asdf.cpp asdf.h
	$(CXX) -c -fPIC $< -o $@ -L. -lprog

libasdf.so: asdf.o
	$(CXX) $< -shared -o $@ $(GL)

hjkl.o: hjkl.cpp hjkl.h
	$(CXX) -c -fPIC $< -o $@

libhjkl.so: hjkl.o
	$(CXX) $< -shared -o $@ -L. -lasdf -lprog

disp.o: disp.cpp disp.h
	$(CXX) -c $< -o $@ $(SDL) $(GL)

util.o: util.cpp util.h
	$(CXX) -c $< -o $@

main.o: main.cpp
	$(CXX) -c $< -o $@ $(PY) -L. -lasdf -lhjkl

$(PROG): $(OBJ_STATIC)
	$(CXX) $^ $(PY) -L. -lasdf -lhjkl -o $(PROG) $(GL) $(SDL)

clean:
	rm *.o *.so $(PROG)
