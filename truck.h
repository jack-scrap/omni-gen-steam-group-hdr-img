#pragma once

#include "obj.h"
#include "pt.h"

typedef struct {
	Obj* _parent;

	Pt* _bed;
} Truck;

Truck* truckMk();

extern "C" void truckDraw(Truck* truck);
