#pragma once

#include "obj.h"

typedef struct Mark {
	GLfloat val[2][2];

	Obj* _parent;

	static GLushort _idc[2];
} Mark;

Mark* markMk(GLfloat val[2][2], unsigned int status);

void markDel(Mark* inst);
