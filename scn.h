#pragma once

#include <vector>

#include "obj.h"
#include "truck.h"
#include "crane.h"
#include "cargo_ship.h"
#include "pt.h"
#include "cam.h"
#include "console.h"
#include "var.h"
#include "arr.h"
#include "lim.h"
#include "cone.h"
#include "street_light.h"

extern Disp* disp;

extern Cam cam;

extern Console* console;

extern Var** data;
extern Var** goal;
extern unsigned int* type;
extern unsigned int noData;
extern bool eq;

extern "C" Var** dataGet();

extern "C" Var** goalGet();

extern "C" unsigned int* typeGet();
extern "C" unsigned int noDataGet();

extern "C" Crane** crane;
extern "C" unsigned int noCrane;

extern "C" Truck** truck;
extern "C" unsigned int noTruck;

extern "C" CargoShip** cargoShip;
extern "C" unsigned int noCargoShip;

extern "C" Crane** craneGet();
extern "C" unsigned int noCraneGet();

extern "C" Truck** truckGet();
extern "C" unsigned int noTruckGet();

extern "C" CargoShip** cargoShipGet();
extern "C" unsigned int noCargoShipGet();

extern std::vector<Obj*> obj;
extern std::vector<Obj*> line;
extern std::vector<Obj*> pt;

namespace scn {
	void init(unsigned int stage, unsigned int lvl);
}

extern "C" StreetLight** streetLight;
extern "C" StreetLight** streetLightGet();

extern "C" unsigned int noStreetLight;
extern "C" unsigned int noStreetLightGet();

extern Lim** boundRng;
extern "C" unsigned int noBoundRng;

extern "C" Cone** boundArea;
extern "C" unsigned int noBoundArea;

extern "C" Lim** boundRngGet();
extern "C" unsigned int noBoundAreaGet();

extern "C" Cone** boundAreaGet();
extern "C" unsigned int noBoundRngGet();

enum type {
	SCALAR,
	ARRAY,
	DICT
};
