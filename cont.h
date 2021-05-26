#pragma once

#include <glm/glm.hpp>

#include "obj.h"

typedef struct {
	char _c;

	GLuint _tex;

	Obj* _parent;
} Cont;

Cont* contMk(char c, glm::vec3 loc = glm::vec3(0.0), glm::vec3 rot = glm::vec3(0.0));
