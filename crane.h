#pragma once

#include <glm/glm.hpp>

#include "obj.h"
#include "cont.h"

typedef struct Crane {
	Cont* _data;
	GLfloat _loc[3];

	GLfloat _rngHead[2] = {
		-3.0,
		3.0
	};
	GLfloat _rngClaw[2] = {
		0.0,
		13.8
	};

	static constexpr GLfloat _speed = 0.1;

	Obj* _parent;

	enum child {
		HEAD = 2 * 2 * 2 * 2,
		CLAW = 0
	};
} Crane;

Crane* craneMk(glm::vec3 loc = glm::vec3(0.0), glm::vec3 rot = glm::vec3(0.0));

extern "C" void craneAnim(Crane* crane, glm::vec3 loc);

extern "C" void craneZoom(Crane* crane, float delta);

extern "C" void cranePan(Crane* crane, float delta);

extern "C" void cranePed(Crane* crane, float delta);

extern "C" void craneGrab(Crane* crane);

void craneInsert(Crane* crane, Cont* byte);
