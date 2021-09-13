#pragma once

#include "obj.h"

typedef struct {
	unsigned int _status;

	Obj* _parent;
} Road;

Road* roadMk(GLfloat* vtc, GLushort* idc, unsigned int noPrim, unsigned int status, glm::vec3 loc = glm::vec3(0.0), glm::vec3 rot = glm::vec3(0.0));

void roadDel(Road* road);
