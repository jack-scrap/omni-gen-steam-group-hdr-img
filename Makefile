CXX = g++

PROG = a.out

STATIC = main disp util mesh bg char console
DYNA = poly obj scn prog

DEP_STATIC = $(patsubst %, %.o, $(STATIC))
DEP_DYNA = $(patsubst %, lib%.so, $(DYNA))

SDL = -lSDL2 -lSDL2_ttf
GL = -lGLEW -lGL
PY = -L/usr/lib/python3.6/config-3.6m-x86_64-linux-gnu -L/usr/lib -lpython3.6m -pthread -ldl  -lutil -lm  -Xlinker -export-dynamic -Wl,-O1 -Wl,-Bsymbolic-functions -I/usr/include/python3.6m -I/usr/include/python3.6m  -g -fdebug-prefix-map=/build/python3.6-0aiVHW/python3.6-3.6.9=. -specs=/usr/share/dpkg/no-pie-compile.specs -fstack-protector -DNDEBUG -g -fwrapv -O3 -fPIC

LDFLAGS += $(SDL)
LDFLAGS += $(GL)
LDFLAGS += $(PY)
LDFLAGS += -L. $(patsubst %, -l%, $(DYNA))

all: $(PROG)

%.o: %.cpp %.h
	$(CXX) -c $< $(LDFLAGS) -o $@

lib%.so: %.o
	$(CC) -shared $< -o $@

main.o: main.cpp
	$(CXX) -c $< $(LDFLAGS) -o $@

$(PROG): $(DEP_STATIC) $(DEP_DYNA)
	$(CXX) $(DEP_STATIC) $(LDFLAGS)

clean:
	rm *.o *.so $(PROG)
