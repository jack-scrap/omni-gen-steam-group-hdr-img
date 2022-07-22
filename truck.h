#pragma once

#include <glm/glm.hpp>

#include "obj.h"
#include "pt.h"
#include "array.h"
#include "c_buff.h"

typedef struct Truck {
	Array* data;

	float offset[3];

	float ang;

	GLuint _uni[2];

	Obj* _parent;

	static constexpr float _rngWheel[2] = {
		-90,
		90
	};

	static GLfloat _bedVtx[3];
	static GLushort _bedIdx[1];

	static GLfloat _lightVtc[2 * 2 * 3];
	static GLushort _lightIdc[2 * 3];

	static constexpr float _speed = 0.3;

	enum {
		BED,
		OUTER,
		TAIL,

		DATA
	};
} Truck;

Truck* truckMk(Array* init, glm::vec3 loc = glm::vec3(0.0), glm::vec3 rot = glm::vec3(0.0));

void truckDel(Truck* inst);

void truckAnim(Truck* inst, glm::vec3 loc, glm::vec3 rot);

extern "C" void truckTurn(Truck* inst, float delta);

extern "C" void truckMv(Truck* inst, float delta);
