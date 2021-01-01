#pragma once

#include <glm/glm.hpp>

#include "obj.h"

typedef struct {
	Obj* _parent;
} Crane;

extern "C" Crane* craneMk(glm::vec3 loc);

extern "C" void craneMv(Crane* crane, GLfloat* d);

extern "C" Crane* craneGet();

extern "C" void craneSet(Crane* crane, GLfloat* d);

extern "C" void craneDraw(Crane* crane);
