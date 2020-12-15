#pragma once

#include "obj.h"

typedef struct {
	Obj* _parent;
} Pt;

Pt* ptMk();

void ptMv(Pt* pt, GLfloat* d);

void ptDraw(Pt* pt);
