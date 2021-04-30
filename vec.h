#pragma once

#include <glm/glm.hpp>

#include "obj.h"

typedef struct {
	Obj* _parent;
} Vec;

Vec* vecMk(glm::vec3 loc = glm::vec3(0.0), glm::vec3 rot = glm::vec3(0.0));
