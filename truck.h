#pragma once

#include <glm/glm.hpp>

#include "obj.h"
#include "pt.h"

typedef struct {
	Obj* _parent;

	Pt* _bed;
} Truck;

Truck* truckMk(glm::vec3 loc);

extern "C" void truckMv(Truck* truck, GLfloat* d);
