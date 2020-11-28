CXX := g++

STATIC := main.cpp disp.cpp util.cpp
DYNA := poly.cpp scn.cpp
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

all: libprog.so libpoly.so libscn.so $(PROG)

prog.o: prog.cpp prog.h
	$(CXX) -c -fPIC $< -o $@

libprog.so: prog.o
	$(CXX) $< -shared -o $@

poly.o: poly.cpp poly.h
	$(CXX) -c -fPIC $< -o $@ -L. -lprog

libpoly.so: poly.o
	$(CXX) $< -shared -o $@ $(GL)

scn.o: scn.cpp scn.h
	$(CXX) -c -fPIC $< -o $@

libscn.so: scn.o
	$(CXX) $< -shared -o $@ -L. -lpoly -lprog

disp.o: disp.cpp disp.h
	$(CXX) -c $< -o $@ $(SDL) $(GL)

util.o: util.cpp util.h
	$(CXX) -c $< -o $@

main.o: main.cpp
	$(CXX) -c $< -o $@ $(PY) -L. -lpoly -lscn

$(PROG): $(OBJ_STATIC)
	$(CXX) $^ $(PY) -L. -lpoly -lscn -o $(PROG) $(GL) $(SDL)

clean:
	rm *.o *.so $(PROG)
