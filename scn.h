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

extern Disp disp;

extern Console* console;

extern Truck* truck;

extern Crane* crane;

extern std::vector<Obj*> scn;

extern Vec* vec;

extern "C" Truck* truckGet();

extern "C" Crane* craneGet();

extern "C" Bound* bound;

extern Arr* data;

extern "C" char rhs[];

extern bool eq;

extern "C" void** asdfGet();

extern "C" int* uiop;

extern "C" int* uiopGet();
