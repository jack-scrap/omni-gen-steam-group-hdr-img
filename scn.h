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

extern "C" Var** data;
extern "C" Var** goal;
extern "C" unsigned int* type;
extern "C" unsigned int noData;
extern "C" bool eq;

extern "C" Var** dataGet();

extern "C" Var** goalGet();

extern "C" unsigned int* typeGet();
extern "C" unsigned int noDataGet();

extern "C" cArr crane;

extern "C" cArr truck;

extern "C" cArr cargoShip;

extern "C" cArr craneGet();

extern "C" cArr truckGet();

extern "C" cArr cargoShipGet();

extern "C" cArr streetLight;
extern "C" cArr streetLightGet();

extern "C" cArr boundRng;
extern "C" cArr boundRngGet();

extern "C" cArr boundArea;
extern "C" cArr boundAreaGet();

namespace scn {
	extern std::vector<Obj*> obj;
	extern std::vector<Obj*> line;
	extern std::vector<Obj*> pt;

	void init(unsigned int stage, unsigned int lvl);
}
