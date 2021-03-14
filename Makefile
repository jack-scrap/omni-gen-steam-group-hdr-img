PROG = omni

CXX = g++

HDR = cam col state math phys

STATIC = main disp util mesh console str border node cont idx arr lim omni
DYNA = scn vec obj truck crane pt prog bound

DEP_STATIC = $(patsubst %, %.o, $(STATIC))
DEP_DYNA = $(patsubst %, lib%.so, $(DYNA))
DEP_HDR = $(patsubst %, %.h, $(HDR))

LDFLAGS += -lSDL2 -lSDL2_ttf
LDFLAGS += -lGLEW -lGL
LDFLAGS += -L/usr/lib/python3.6/config-3.6m-x86_64-linux-gnu -L/usr/lib -lpython3.6m -pthread -ldl  -lutil -lm  -Xlinker -export-dynamic -Wl,-O1 -Wl,-Bsymbolic-functions -I/usr/include/python3.6m -I/usr/include/python3.6m  -g -fdebug-prefix-map=/build/python3.6-0aiVHW/python3.6-3.6.9=. -specs=/usr/share/dpkg/no-pie-compile.specs -fstack-protector -DNDEBUG -g -fwrapv -O3 -fPIC
LDFLAGS += -L. $(patsubst %, -l%, $(DYNA))

all: $(PROG)

%.o: %.cpp %.h
	$(CXX) -c $< -o $@ $(LDFLAGS)

lib%.so: %.o
	$(CC) -shared $< -o $@

main.o: main.cpp
	$(CXX) -c $< -o $@ $(LDFLAGS)

$(PROG): $(DEP_STATIC) $(DEP_DYNA) $(DEP_HDR)
	$(CXX) $(DEP_STATIC) $(LDFLAGS) -o $@

clean:
	rm *.o *.so $(PROG)
