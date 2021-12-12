#pragma once

#include <glm/glm.hpp>

#include "obj.h"
#include "cont.h"

typedef struct Crane {
	Cont* _data;
	float _loc[3];

	const float _rngHead[2] = {
		-3.0,
		3.0
	};
	const float _rngClaw[2] = {
		0.0,
		13.8
	};

	Obj* _parent;

	static GLfloat _speed;

	enum {
		HEAD,
		SLOT,

		CLAW = 0
	};
} Crane;

Crane* craneMk(Cont* init, glm::vec3 loc = glm::vec3(0.0), glm::vec3 rot = glm::vec3(0.0));

void craneDel(Crane* crane);

extern "C" void craneAnim(Crane* crane, glm::vec3 loc);

extern "C" void craneZoom(Crane* crane, float delta);

extern "C" void cranePan(Crane* crane, float delta);

extern "C" void cranePed(Crane* crane, float delta);

extern "C" void craneGrab(Crane* crane);

void craneInsert(Crane* crane, Cont* byte);
