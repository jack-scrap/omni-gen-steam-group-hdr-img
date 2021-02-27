#pragma once

#include <glm/glm.hpp>

#include "border.h"

typedef struct {
	Obj* _parent;
} Idx;

Idx* idxMk(unsigned int i, glm::vec3 loc = glm::vec3(0.0, 0.0, 0.0), glm::vec3 rot = glm::vec3(0.0, 0.0, 0.0));
