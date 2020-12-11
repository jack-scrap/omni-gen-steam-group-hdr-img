#pragma once

#include "obj.h"

typedef struct {
	Obj* _parent;
} Pt;

Pt* ptMk();

void ptDraw(Pt* pt);
