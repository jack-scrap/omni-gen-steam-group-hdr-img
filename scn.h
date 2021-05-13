#pragma once

#include <vector>

#include "obj.h"
#include "truck.h"
#include "crane.h"
#include "cargo_ship.h"
#include "pt.h"
#include "cam.h"
#include "console.h"
#include "bound.h"
#include "var.h"
#include "arr.h"
#include "lim.h"
#include "cone.h"

extern Disp* disp;

extern Cam cam;

extern Console* console;

extern void** data;
extern void** goal;
extern unsigned int* type;
extern unsigned int noData;
extern bool eq;

extern "C" void** dataGet();

extern "C" void** goalGet();

extern "C" unsigned int noDataGet();

extern Lim** boundRng;
extern "C" unsigned int noBoundRng;

extern "C" Cone** boundArea;
extern "C" unsigned int noBoundArea;

extern "C" Lim** boundRngGet();

extern "C" unsigned int noBoundRngGet();

extern "C" Cone** boundAreaGet();

extern "C" unsigned int noBoundAreaGet();

extern "C" Crane** crane;
extern "C" Truck** truck;
extern "C" CargoShip** cargoShip;

extern "C" unsigned int noCrane;
extern "C" unsigned int noTruck;
extern "C" unsigned int noCargoShip;

extern "C" Crane** craneGet();

extern "C" Truck** truckGet();

extern "C" CargoShip** cargoShipGet();

extern "C" unsigned int noCraneGet();

extern "C" unsigned int noTruckGet();

extern "C" unsigned int noCargoShipGet();

extern std::vector<Obj*> mesh;
extern std::vector<Obj*> line;
extern std::vector<Obj*> pt;

namespace scn {
	void init(unsigned int stage, unsigned int lvl);
}

enum type {
	SCALAR,
	ARRAY,
	DICT
};
