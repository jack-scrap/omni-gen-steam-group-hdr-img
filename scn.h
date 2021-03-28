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

namespace scn {
	void init(unsigned int i);
}
