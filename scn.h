#pragma once

#include <vector>

#include "obj.h"
#include "truck.h"
#include "crane.h"
#include "pt.h"
#include "cam.h"
#include "console.h"
#include "bound.h"
#include "vec.h"
#include "data.h"
#include "cone.h"
#include "lim.h"

extern Disp* disp;

extern Cam cam;

extern Console* console;

extern void** data;
extern unsigned int s;
extern char* goal;
extern bool eq;

extern "C" unsigned int szGet();

extern Lim* boundRng;
extern "C" unsigned int r;

extern "C" void** coneRng;
extern "C" unsigned int c;

extern "C" void* boundRngGet();

extern "C" void** coneRngGet();

extern std::vector<void*> vehicle;

extern "C" void** vehicleGet();

extern std::vector<Obj*> mesh;
extern std::vector<Obj*> pt;

extern "C" void** dataGet();

extern "C" void* goalGet();

namespace scn {
	void init(unsigned int stage, unsigned int lvl);
}
