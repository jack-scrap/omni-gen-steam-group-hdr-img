#pragma once

#include <vector>

#include "obj.h"

typedef struct Aabb {
	Obj* _parent;

	static GLushort _idc[3 * 2 * 3 * 2];
} Aabb;

Aabb* aabbMK(Obj** obj, unsigned int sz);
