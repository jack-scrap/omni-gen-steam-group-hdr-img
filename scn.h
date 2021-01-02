#pragma once

#include <vector>

#include "poly.h"
#include "obj.h"
#include "truck.h"
#include "crane.h"
#include "pt.h"
#include "cam.h"
#include "console.h"

extern Cam cam;

extern Disp disp;

extern Console* console;

extern Truck* truck;

extern Crane* crane;

extern std::vector<Obj*> obj;

extern "C" Truck* truckGet();

extern "C" void truckSet(Truck* truck, GLfloat* d);

extern "C" Crane* craneGet();

extern "C" void craneSet(Crane* crane, GLfloat* d);
