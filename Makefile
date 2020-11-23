CXX := g++

STATIC := main.cpp util.cpp
DYNA := asdf.cpp hjkl.cpp
OBJ_STATIC += $(STATIC:.cpp=.o)
OBJ_DYNA += $(DYNA:.cpp=.o)

LIB = -L. $(DYNA:%.cpp=-l%)
PY = -L/usr/lib/python3.6/config-3.6m-x86_64-linux-gnu -L/usr/lib -lpython3.6m -pthread -ldl  -lutil -lm  -Xlinker -export-dynamic -Wl,-O1 -Wl,-Bsymbolic-functions -I/usr/include/python3.6m -I/usr/include/python3.6m  -g -fdebug-prefix-map=/build/python3.6-0aiVHW/python3.6-3.6.9=. -specs=/usr/share/dpkg/no-pie-compile.specs -fstack-protector -DNDEBUG -g -fwrapv -O3 -fPIC
LDFLAGS += $(LIB)
LDFLAGS += $(PY)

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

util.o: util.cpp
	$(CXX) -c $< -o $@

main.o: main.cpp
	$(CXX) -c $< -o $@ $(PY) -L. -lasdf -lhjkl

$(PROG): $(OBJ_STATIC)
	$(CXX) $^ $(PY) -L. -lasdf -lhjkl -o $(PROG)

clean:
	rm *.o *.so $(PROG)
