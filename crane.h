#pragma once

#include <glm/glm.hpp>

#include "obj.h"

typedef struct {
	Obj* _parent;

	Obj* _op;
} Crane;

Crane* craneMk(glm::vec3 loc);

void craneUpdate(Obj* obj, glm::vec3 dir);

void craneAnim(Crane* crane, GLfloat* dest);

extern "C" void craneMv(Crane* crane, GLfloat* dest);

extern "C" void cranePan(Crane* crane, bool dir);

extern "C" void cranePed(Crane* crane, bool dir);

extern "C" void craneGrab(Crane* crane);

void craneDraw(Crane* crane);
