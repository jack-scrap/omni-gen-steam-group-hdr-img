#pragma once

#include <glm/glm.hpp>

#include "obj.h"

typedef struct Crane {
	static constexpr GLfloat _speed = 0.1;

	Obj* _parent;

	Obj* _op;

	static constexpr GLfloat _rngHead[2] = {
		-3.0,
		3.0
	};
	static constexpr GLfloat _rngClaw[2] = {
		0.0,
		13.8
	};
} Crane;

Crane* craneMk(glm::vec3 loc = glm::vec3(0.0), glm::vec3 rot = glm::vec3(0.0));

extern "C" void craneAnim(Crane* crane, GLfloat* loc);

extern "C" void craneMv(Crane* crane, GLfloat* loc);

extern "C" void cranePan(Crane* crane, bool dir);

extern "C" void cranePed(Crane* crane, bool dir);

extern "C" void craneGrab(Crane* crane);
