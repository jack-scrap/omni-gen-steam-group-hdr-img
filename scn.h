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
#include "array.h"
#include "lim.h"
#include "cone.h"
#include "street_sign.h"

extern Disp* disp;

extern Cam cam;

extern Console* console;

extern "C" Var** data;
extern "C" Var** goal;
extern "C" unsigned int noData;

extern "C" bool eq;

extern "C" bool mtx;

extern "C" bool adv;

extern "C" Var** dataGet();

extern "C" Var** goalGet();

extern "C" unsigned int noDataGet();

extern "C" CArr crane;
extern "C" CArr craneGet();

extern "C" CArr truck;
extern "C" CArr truckGet();

extern "C" CArr cargoShip;
extern "C" CArr cargoShipGet();

extern "C" CArr streetSign;
extern "C" CArr streetSignGet();

extern "C" CArr boundRng;
extern "C" CArr boundRngGet();

extern "C" CArr boundArea;
extern "C" CArr boundAreaGet();

extern "C" CArr node;
extern "C" CArr nodeGet();

extern "C" CArr path;
extern "C" CArr pathGet();

extern "C" unsigned int call;
extern "C" void callIncr();

namespace scn {
	extern std::vector<Obj*> obj;

	void init(std::string stage, unsigned int lvl);
}
