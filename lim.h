#pragma once

#include "obj.h"

typedef struct Lim {
	Obj* _parent;

	unsigned int axis;
	float val;

	static GLushort _idc[2];
} Lim;

Lim* limMk(unsigned int axis, GLfloat val, unsigned int status);

void limDel(Lim* inst);
