#pragma once

#include "obj.h"

typedef struct {
	Obj* _parent;
} Dict;

Dict* dictMk(void* data, unsigned int no, std::string name = "", glm::vec3 loc = glm::vec3(0.0), glm::vec3 rot = glm::vec3(0.0));
