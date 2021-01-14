#pragma once

#include <glm/glm.hpp>

#include "obj.h"
#include "pt.h"

typedef struct {
	Obj* _parent;
} Truck;

Truck* truckMk(glm::vec3 loc = glm::vec3(0.0, 0.0, 0.0), glm::vec3 rot = glm::vec3(0.0, 0.0, 0.0));
