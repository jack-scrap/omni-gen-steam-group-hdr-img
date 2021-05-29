#pragma once

#include "obj.h"

typedef struct Lim {
	unsigned int _axis;
	GLfloat _val;

	static GLushort _idc[2];

	Obj* _parent;

	enum {
		PASS,
		HALT,
		ALERT
	};
} Lim;

Lim* limMk(unsigned int axis, GLfloat val);
