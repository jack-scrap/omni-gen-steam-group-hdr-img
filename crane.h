#pragma once

#include <glm/glm.hpp>

#include "obj.h"
#include "cont.h"

typedef struct Crane {
	Cont* data;

	float offset[3];

	Obj* _parent;

	static constexpr float _rngTrack[2] = {
		4.0,
		13.8
	};
	static constexpr float _rngHead[2] = {
		-3.0,
		3.0
	};

	static GLfloat _lightVtc[2 * 2 * 3];
	static GLushort _lightIdc[2 * 3];

	static constexpr float _speed = 0.2;

	enum {
		TRACK,
		HEAD = 0,

		DATA = 0
	};
} Crane;

Crane* craneMk(Cont* init, glm::vec3 loc = glm::vec3(0.0), glm::vec3 rot = glm::vec3(0.0));

void craneDel(Crane* inst);

extern "C" void craneAnim(Crane* inst, glm::vec3 loc);

extern "C" void craneZoom(Crane* inst, float delta);

extern "C" void cranePan(Crane* inst, float delta);

extern "C" void cranePed(Crane* inst, float delta);

extern "C" void craneGrab(Crane* inst);

void craneInsert(Crane* inst, Cont* byte);

Cont* craneRm(Crane* inst);
