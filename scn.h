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
extern std::vector<char> rhs;
extern bool eq;

extern std::vector<void*> vehicle;

extern "C" void** vehicleGet();

extern std::vector<Obj*> mesh;
extern std::vector<Obj*> pt;

namespace scn {
	void init(unsigned int stage, unsigned int lvl);
}

extern "C" void* dataGet();

typedef struct Attr {
	char* _ptr;
	int _x;
	int _y;

	float _loc[3];
} Attr;

extern "C" Attr attr;

extern "C" void* attrGet();
