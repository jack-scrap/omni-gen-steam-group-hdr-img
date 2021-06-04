#pragma once

#include <glm/glm.hpp>

#include "obj.h"
#include "pt.h"
#include "arr.h"
#include "c_buff.h"

typedef struct Truck {
	Arr* _data;
	GLfloat _loc[3];

	float _ang;

	GLfloat _rngWheel[2] = {
		-(M_PI / 2),
		M_PI / 2
	};

	Obj* _parent;

	GLuint _uni[2];

	static GLfloat _vtx[3];
	static GLushort _idx[1];

	static GLfloat _speed;

	enum {
		BED,
		OUTER,
		TAIL,
		DATA
	};
} Truck;

Truck* truckMk(CArr init = {
	nullptr,
	0
}, glm::vec3 loc = glm::vec3(0.0), glm::vec3 rot = glm::vec3(0.0));

void truckAnim(Truck* truck, glm::vec3 loc, glm::vec3 rot);

extern "C" void truckTurn(Truck* truck, float delta);

extern "C" void truckMv(Truck* truck, float delta);
