#pragma once

#include "obj.h"

typedef struct {
	Obj* _parent;
} Bound;

Bound* boundMk();
