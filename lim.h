#pragma once

#include "obj.h"

typedef struct Lim {
	unsigned int _axis;
	GLfloat _val;

	Obj* _parent;

	static GLushort _idc[2];
} Lim;

Lim* limMk(unsigned int axis, GLfloat val, unsigned int status);

void limDel(Lim* lim);
