#pragma once

#include "obj.h"

typedef struct {
	unsigned int _axis;
	GLfloat _val;

	Obj* _parent;
} Lim;

Lim* limMk(unsigned int axis, GLfloat val, glm::vec3 loc = glm::vec3(0.0), glm::vec3 rot = glm::vec3(0.0));
