#pragma once

#include <glm/glm.hpp>

#include "obj.h"

typedef struct {
	Obj* _parent;
} Pt;

Pt* ptMk(glm::vec3 loc);

void ptMv(Pt* pt, GLfloat* d);

void ptDraw(Pt* pt);
