#pragma once

#include <cstdlib>
#include <glm/glm.hpp>

#include "obj.h"

typedef struct {
	Obj* _parent;

	glm::vec2 _sz;
} Border;

Border* borderMk(glm::vec2 sz, glm::vec3 loc = glm::vec3(0.0, 0.0, 0.0), glm::vec3 rot = glm::vec3(0.0, 0.0, 0.0));
