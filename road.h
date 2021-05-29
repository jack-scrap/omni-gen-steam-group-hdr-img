#pragma once

#include "obj.h"

typedef struct {
	unsigned int _pass;

	Obj* _parent;
} Road;

Road* roadMk(GLfloat* vtc, unsigned int noVtc, GLushort* idc, unsigned int noPrim, unsigned int pass, glm::vec3 loc = glm::vec3(0.0), glm::vec3 rot = glm::vec3(0.0));
