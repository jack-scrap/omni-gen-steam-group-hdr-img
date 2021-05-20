PROG = omni

CXX = g++
CXXFLAGS = -std=c++14 -Wno-narrowing

HDR = cam col math phys

STATIC = main disp util mesh console var str border node cont idx arr dict lim cone road state layout omni
DYNA = scn obj line crane truck cargo_ship pt prog street_light

DEP_HDR = $(patsubst %, %.h, $(HDR))
OBJ_STATIC = $(patsubst %, %.o, $(STATIC))
OBJ_DYNA = $(patsubst %, lib%.so, $(DYNA))

LDSDL += -lSDL2 -lSDL2_ttf
LDGL += -lGLEW -lGL
LDPY += -L/usr/lib/python3.6/config-3.6m-x86_64-linux-gnu -L/usr/lib -lpython3.6m -pthread -ldl -lutil -lm -Xlinker -export-dynamic -Wl,-O1 -Wl,-Bsymbolic-functions -I/usr/include/python3.6m -I/usr/include/python3.6m -g -fdebug-prefix-map=/build/python3.6-0aiVHW/python3.6-3.6.9=. -specs=/usr/share/dpkg/no-pie-compile.specs -fstack-protector -DNDEBUG -g -fwrapv -O3 -fPIC
LDLIB += -L. $(patsubst %, -l%, $(DYNA))

STAGE = init ctrl_flow thread path matrix

all: $(PROG) ro mk_stage

%.o: %.cpp %.h
	$(CXX) $(CXXFLAGS) -c $< -o $@

scn.o: scn.cpp scn.h
	$(CXX) $(CXXFLAGS) -c $< -o $@

console.o: console.cpp console.h
	$(CXX) $(CXXFLAGS) -c $< -o $@ $(LDPY)

scn.o: scn.cpp scn.h
	$(CXX) $(CXXFLAGS) -fPIC -c $< -o $@

obj.o: obj.cpp obj.h
	$(CXX) $(CXXFLAGS) -fPIC -c $< -o $@

line.o: line.cpp line.h
	$(CXX) $(CXXFLAGS) -fPIC -c $< -o $@

crane.o: crane.cpp crane.h
	$(CXX) $(CXXFLAGS) -fPIC -c $< -o $@

truck.o: truck.cpp truck.h
	$(CXX) $(CXXFLAGS) -fPIC -c $< -o $@

cargo_ship.o: cargo_ship.cpp cargo_ship.h
	$(CXX) $(CXXFLAGS) -fPIC -c $< -o $@

pt.o: pt.cpp pt.h
	$(CXX) $(CXXFLAGS) -fPIC -c $< -o $@

prog.o: prog.cpp prog.h
	$(CXX) $(CXXFLAGS) -fPIC -c $< -o $@

bound.o: bound.cpp bound.h
	$(CXX) $(CXXFLAGS) -fPIC -c $< -o $@

street_light.o: street_light.cpp street_light.h
	$(CXX) $(CXXFLAGS) -fPIC -c $< -o $@

lib%.so: %.o
	$(CXX) $(CXXFLAGS) -shared $< -o $@

main.o: main.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(PROG): $(OBJ_STATIC) $(OBJ_DYNA) $(DEP_HDR)
	$(CXX) $(CXXFLAGS) $(OBJ_STATIC) -o $@ $(LDSDL) $(LDGL) $(LDPY) $(LDLIB)

ro:
	chmod -w doc/*

mk_stage:
	for DIR in $(STAGE) ; do \
		mkdir -p lvl/$$DIR ; \
	done

clean:
	rm *.o *.so $(PROG)
