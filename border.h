#pragma once

#include <cstdlib>
#include <glm/glm.hpp>

#include "obj.h"

typedef struct {
	glm::vec2 _bound;

	Obj* _parent;
} Border;

Border* borderMk(glm::vec2 bound, glm::vec3 loc = glm::vec3(0.0), glm::vec3 rot = glm::vec3(0.0));

Border* borderMk(glm::vec2 bound, Obj** child, unsigned int noChild, glm::vec3 loc = glm::vec3(0.0), glm::vec3 rot = glm::vec3(0.0));

void borderDel(Border* border);
