#pragma once

#include "obj.h"

typedef struct {
	Obj* _parent;
} Str;

Str* strMk(std::string str, glm::vec3 loc = glm::vec3(0.0), glm::vec3 rot = glm::vec3(0.0));
