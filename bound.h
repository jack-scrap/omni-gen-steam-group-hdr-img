#pragma once

#include <vector>

#include "obj.h"

typedef struct {
	Obj* _parent;
} Bound;

Bound* boundMk(std::vector<Obj*> scn);
