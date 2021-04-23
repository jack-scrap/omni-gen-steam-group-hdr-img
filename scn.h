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
#include "arr.h"

extern Cam cam;

extern Disp* disp;

extern Console* console;

extern Arr* data;
extern unsigned int sz;
extern char* rhs;
extern bool eq;

extern std::vector<void*> vehicle;

extern "C" void** vehicleGet();

extern std::vector<Obj*> mesh;
extern std::vector<Obj*> pt;

namespace scn {
	void init(unsigned int stage, unsigned int lvl);
}

typedef struct Attr {
	char* _ptr;
	unsigned int _x;
	unsigned int _y;

	float _loc[3];
} Attr;

extern "C" Attr attr;

extern "C" void* attrGet();

extern "C" void* rhsGet();
