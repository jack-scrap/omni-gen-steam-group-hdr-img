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

extern "C" void* boundRngGet();

extern std::vector<void*> vehicle;

extern "C" void** vehicleGet();

extern std::vector<Obj*> mesh;
extern std::vector<Obj*> pt;

typedef struct Attr {
	char* _ptr;
	unsigned int _x;
	unsigned int _y;

	float _loc[3];
} Attr;

extern "C" Attr attr;

extern "C" void* attrGet();

extern "C" void* rhsGet();

namespace scn {
	void init(unsigned int stage, unsigned int lvl);
}
