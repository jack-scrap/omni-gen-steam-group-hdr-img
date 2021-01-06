#pragma once

#include <glm/glm.hpp>

#include "obj.h"

typedef struct {
	Obj* _parent;

	Obj* _op;
} Crane;

extern "C" Crane* craneMk(glm::vec3 loc);

extern "C" void craneAnim(Crane* crane, GLfloat* d);

extern "C" void craneMv(Crane* crane, GLfloat* d);

extern "C" void cranePan(Crane* crane, bool dir);

extern "C" void cranePed(Crane* crane, bool dir);

extern "C" void craneGrab(Crane* crane);

extern "C" void craneDraw(Crane* crane);
