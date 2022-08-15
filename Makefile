SH:=/bin/bash

CXX:=g++

BUILDDIR:=build

DESTDIR:=/usr/local/bin
LIBDIR:=/usr/lib

CXXFLAGS:=-std=c++11 -Wno-narrowing

SDLFLAGS:=-lSDL2 -lSDL2_ttf -lSDL2_image
GLFLAGS:=-lGLEW -lGL
PYFLAGS:=-L/usr/lib/python3.6/config-3.6m-x86_64-linux-gnu -L/usr/lib -lpython3.6m -pthread -ldl -lutil -lm -Xlinker -export-dynamic -Wl,-O1 -Wl,-Bsymbolic-functions -I/usr/include/python3.6m -I/usr/include/python3.6m -g -fdebug-prefix-map=/build/python3.6-0aiVHW/python3.6-3.6.9=. -specs=/usr/share/dpkg/no-pie-compile.specs -fstack-protector -DNDEBUG -g -fwrapv -O3 -fPIC
LDLIBS=-L$(BUILDDIR) -Wl,-rpath=$(BUILDDIR) $(LIBS:%.cpp=-l%)

SRCS:=main.cpp disp.cpp util.cpp mesh.cpp console.cpp var.cpp str.cpp border.cpp node.cpp cont.cpp idx.cpp array.cpp dict.cpp mark.cpp lim.cpp cone.cpp road.cpp state.cpp layout.cpp i_beam.cpp omni.cpp c_buff.cpp
LIBS:=scn.cpp prog.cpp obj.cpp line.cpp pt.cpp crane.cpp truck.cpp cargo_ship.cpp street_sign.cpp

HDR:=cam.h col.h math.h phys.h

OBJ_STATIC=$(SRCS:%.cpp=$(BUILDDIR)/%.o)
OBJ_DYNA=$(LIBS:%.cpp=$(BUILDDIR)/lib%.so)

STAGE:=init array str dict matrix vec ctrl_flow path thread

.PHONY: all
all: mk_build mk_script omni ro

$(BUILDDIR)/main.o: main.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@ $(PYFLAGS)

$(BUILDDIR)/%.o: %.cpp %.h
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(BUILDDIR)/%.o: %.cpp %.h
	$(CXX) $(CXXFLAGS) -fPIC -c $< -o $@

$(BUILDDIR)/console.o: console.cpp console.h
	$(CXX) $(CXXFLAGS) -c $< -o $@ $(PYFLAGS)

$(BUILDDIR)/lib%.so: $(BUILDDIR)/%.o
	$(CXX) $(CXXFLAGS) -shared $< -o $@

omni: $(OBJ_STATIC) $(OBJ_DYNA) $(HDR)
	$(CXX) $(CXXFLAGS) $(OBJ_STATIC) -o $@ $(SDLFLAGS) $(GLFLAGS) $(PYFLAGS) $(LDLIBS)

.PHONY: ro
ro:
	chmod -w -R player/doc

.PHONY: mk_build
mk_build:
	mkdir -p $(BUILDDIR)

.PHONY: mk_script
mk_script:
	mkdir -p player/script

.PHONY: install
install:
	sudo mv omni $(DESTDIR) ;
	sudo mv build/*.so $(LIBDIR)

.PHONY: clean
clean:
	rm $(BUILDDIR)/*.o $(BUILDDIR)/*.so omni
