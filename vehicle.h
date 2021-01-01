#pragma once

#include "obj.h"

typedef struct {
	Obj* _parent;
} Vehicle;

Vehicle* vehicleMk(glm::vec3 loc);
