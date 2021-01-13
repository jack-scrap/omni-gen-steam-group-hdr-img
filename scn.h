#pragma once

#include <vector>

#include "obj.h"
#include "truck.h"
#include "crane.h"
#include "pt.h"
#include "cam.h"
#include "console.h"
#include "bound.h"

extern Cam cam;

extern Disp disp;

extern Console* console;

extern Truck* truck;

extern Crane* crane;

extern std::vector<Obj*> obj;

extern "C" Truck* truckGet();

extern "C" Bound* bound;

extern "C" Crane* craneGet();
