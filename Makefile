CXX=g++

CXXFLAGS=-std=c++11 -Wno-narrowing

SDLFLAGS=-lSDL2 -lSDL2_ttf -lSDL2_image
GLFLAGS=-lGLEW -lGL
PYFLAGS=-L/usr/lib/python3.6/config-3.6m-x86_64-linux-gnu -L/usr/lib -lpython3.6m -pthread -ldl -lutil -lm -Xlinker -export-dynamic -Wl,-O1 -Wl,-Bsymbolic-functions -I/usr/include/python3.6m -I/usr/include/python3.6m -g -fdebug-prefix-map=/build/python3.6-0aiVHW/python3.6-3.6.9=. -specs=/usr/share/dpkg/no-pie-compile.specs -fstack-protector -DNDEBUG -g -fwrapv -O3 -fPIC
LIBFLAGS=-L./ -Wl,-rpath=./ $(LIBS:%.cpp=-l%)

STATIC=main.cpp disp.cpp util.cpp mesh.cpp console.cpp var.cpp str.cpp border.cpp node.cpp cont.cpp idx.cpp array.cpp dict.cpp lim.cpp cone.cpp road.cpp state.cpp layout.cpp i_beam.cpp omni.cpp
LIBS=scn.cpp prog.cpp obj.cpp line.cpp pt.cpp crane.cpp truck.cpp cargo_ship.cpp street_sign.cpp

HDR=cam.h col.h math.h phys.h

OBJ_STATIC=$(STATIC:%.cpp=%.o)
OBJ_LIBS=$(LIBS:%.cpp=lib%.so)

STAGE=init array str dict matrix vec ctrl_flow path thread

.PHONY: all ro mk_stage clean

all: omni ro mk_stage

install:
	su - && \
	apt-get install sudo -y && \
	apt-get update && \
	apt-get install -y software-properties-common && \
	sudo apt-get install -y libsdl2-dev libsdl2-image-dev libsdl2-ttf-dev mesa-utils libglew-dev libglm-dev python3-dev

main.o: main.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@ $(PYFLAGS)

%.o: %.cpp %.h
	$(CXX) $(CXXFLAGS) -c $< -o $@

console.o: console.cpp console.h
	$(CXX) $(CXXFLAGS) -c $< -o $@ $(PYFLAGS)

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

omni: $(OBJ_STATIC) $(OBJ_LIBS) $(HDR)
	$(CXX) $(CXXFLAGS) $(OBJ_STATIC) -o $@ $(SDLFLAGS) $(GLFLAGS) $(PYFLAGS) $(LIBFLAGS)

ro:
	chmod -w -R player/doc

mk_stage:
	for DIR in $(STAGE) ; do \
		mkdir -p player/script/$$DIR ; \
		chmod -w -R player/script/$$DIR ; \
	done

clean:
	rm *.o *.so omni
