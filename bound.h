#pragma once

#include <vector>

#include "obj.h"

typedef struct {
	Obj* _parent;
} Bound;

Bound* boundMk(Obj** obj, unsigned int sz);
