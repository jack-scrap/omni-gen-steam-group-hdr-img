#pragma once

#include "obj.h"

typedef struct {
	Obj* _parent;
} Truck;

Truck* truckMk();

extern "C" void truckDraw(Truck* truck);
