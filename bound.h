#pragma once

#include <vector>

#include "obj.h"

typedef struct Bound {
	Obj* _parent;

	static GLushort _idc[3 * 2 * 3 * 2];
} Bound;

Bound* boundMk(Obj** obj, unsigned int sz);
