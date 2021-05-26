#pragma once

#include "obj.h"
#include "var.h"
#include "var.h"

typedef struct {
	Obj* _parent;
} Dict;

Dict* dictMk(void** init, unsigned int* type, unsigned int no, std::string name = "", glm::vec3 loc = glm::vec3(0.0), glm::vec3 rot = glm::vec3(0.0));
