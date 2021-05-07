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

extern Disp* disp;

extern Cam cam;

extern Console* console;

extern void** data;
extern unsigned int noData;
extern void** goal;
extern bool eq;

extern "C" void** dataGet();

extern "C" unsigned int noDataGet();

extern "C" void** goalGet();

extern Lim* boundRng;
extern "C" unsigned int r;

extern "C" void** areaRng;
extern "C" unsigned int a;

extern "C" void* boundRngGet();

extern "C" void** areaRngGet();

extern std::vector<Crane*> crane;
extern std::vector<Truck*> truck;
extern std::vector<CargoShip*> cargoShip;

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
