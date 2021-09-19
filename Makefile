CXX=g++
CXXFLAGS=-std=c++11 -Wno-narrowing

LDSDL+=-lSDL2 -lSDL2_ttf -lSDL2_image
LDGL+=-lGLEW -lGL
LDPY+=-L/usr/lib/python3.6/config-3.6m-x86_64-linux-gnu -L/usr/lib -lpython3.6m -pthread -ldl -lutil -lm -Xlinker -export-dynamic -Wl,-O1 -Wl,-Bsymbolic-functions -I/usr/include/python3.6m -I/usr/include/python3.6m -g -fdebug-prefix-map=/build/python3.6-0aiVHW/python3.6-3.6.9=. -specs=/usr/share/dpkg/no-pie-compile.specs -fstack-protector -DNDEBUG -g -fwrapv -O3 -fPIC
LDLIB+=-L. $(patsubst %.c, -l%, $(DYNA))

STATIC=main.c disp.c util.c mesh.c console.c var.c str.c border.c node.c cont.c idx.c array.c dict.c lim.c cone.c road.c state.c layout.c i_beam.c omni.c
DYNA=scn.c prog.c obj.c line.c pt.c crane.c truck.c cargo_ship.c street_sign.c

HDR=cam.h col.h math.h phys.h

OBJ_STATIC=$(patsubst %.c, %.o, $(STATIC))
OBJ_DYNA=$(patsubst %.c, lib%.so, $(DYNA))

STAGE=init ctrl_flow thread path matrix

.PHONY: all ro mk_stage clean

all: omni ro mk_stage

main.o: main.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@ $(LDPY)

%.o: %.cpp %.h
	$(CXX) $(CXXFLAGS) -c $< -o $@

console.o: console.cpp console.h
	$(CXX) $(CXXFLAGS) -c $< -o $@ $(LDPY)

scn.o: scn.cpp scn.h
	$(CXX) $(CXXFLAGS) -fPIC -c $< -o $@

prog.o: prog.cpp prog.h
	$(CXX) $(CXXFLAGS) -fPIC -c $< -o $@

obj.o: obj.cpp obj.h
	$(CXX) $(CXXFLAGS) -fPIC -c $< -o $@

line.o: line.cpp line.h
	$(CXX) $(CXXFLAGS) -fPIC -c $< -o $@

pt.o: pt.cpp pt.h
	$(CXX) $(CXXFLAGS) -fPIC -c $< -o $@

crane.o: crane.cpp crane.h
	$(CXX) $(CXXFLAGS) -fPIC -c $< -o $@

truck.o: truck.cpp truck.h
	$(CXX) $(CXXFLAGS) -fPIC -c $< -o $@

cargo_ship.o: cargo_ship.cpp cargo_ship.h
	$(CXX) $(CXXFLAGS) -fPIC -c $< -o $@

street_sign.o: street_sign.cpp street_sign.h
	$(CXX) $(CXXFLAGS) -fPIC -c $< -o $@

lib%.so: %.o
	$(CXX) $(CXXFLAGS) -shared $< -o $@

omni: $(OBJ_STATIC) $(OBJ_DYNA) $(HDR)
	$(CXX) $(CXXFLAGS) $(OBJ_STATIC) -o $@ $(LDSDL) $(LDGL) $(LDPY) $(LDLIB)

ro:
	chmod -w player/doc

mk_stage:
	for DIR in $(STAGE) ; do \
		mkdir -p lvl/$$DIR ; \
	done

clean:
	rm *.o *.so $(PROG)
