#pragma once

#include <glm/glm.hpp>

#include "obj.h"

typedef struct Crane {
	static constexpr GLfloat _speed = 0.1;

	Obj* _parent;

	Obj* _op;
} Crane;

Crane* craneMk(glm::vec3 loc);

extern "C" void craneAnim(Crane* crane, GLfloat* loc);

extern "C" void craneMv(Crane* crane, GLfloat* loc);

extern "C" void cranePan(Crane* crane, bool dir);

extern "C" void cranePed(Crane* crane, bool dir);

extern "C" void craneGrab(Crane* crane);
