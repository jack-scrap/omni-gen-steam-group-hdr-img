#pragma once

#include <cstdlib>
#include <glm/glm.hpp>

#include "obj.h"

typedef struct {
	glm::vec2 _sz;

	Obj* _parent;
} Border;

Border* borderMk(glm::vec2 sz, glm::vec3 loc = glm::vec3(0.0), glm::vec3 rot = glm::vec3(0.0));

Border* borderMk(glm::vec2 sz, Obj** child, unsigned int noChild, glm::vec3 loc = glm::vec3(0.0), glm::vec3 rot = glm::vec3(0.0));

void borderDel(Border* border);
