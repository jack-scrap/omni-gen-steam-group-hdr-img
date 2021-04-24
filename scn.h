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

extern Cam cam;

extern Disp* disp;

extern Console* console;

extern Data* data;
extern unsigned int sz;
extern char* rhs;
extern bool eq;

typedef struct End {
	unsigned int _axis;

	float _val;
} End;

extern End* boundRng;
extern "C" unsigned int r;

extern "C" float* coneRng;
extern "C" unsigned int c;

extern "C" void* boundRngGet();

extern "C" void* coneRngGet();

extern std::vector<void*> vehicle;

extern "C" void** vehicleGet();

extern std::vector<Obj*> mesh;
extern std::vector<Obj*> pt;

extern "C" void* rhsGet();

extern "C" void* dataGet();

namespace scn {
	void init(unsigned int stage, unsigned int lvl);
}
