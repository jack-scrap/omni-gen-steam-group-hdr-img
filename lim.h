#pragma once

#include "obj.h"

typedef struct {
	unsigned int _axis;
	GLfloat _val;

	Obj* _parent;
} Lim;

Lim* limMk(unsigned int axis, GLfloat val);
