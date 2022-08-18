#pragma once

#include "obj.h"

typedef struct Mark {
	Obj* _parent;

	GLfloat val[2][2];

	static GLushort _idc[2];
} Mark;

Mark* markMk(GLfloat val[2][2], unsigned int status);

void markDel(Mark* inst);
