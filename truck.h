#pragma once

#include <glm/glm.hpp>

#include "obj.h"
#include "pt.h"
#include "array.h"
#include "c_buff.h"

typedef struct Truck {
	Array* _data;
	float _loc[3];

	float _ang;

	float _rngWheel[2] = {
		-(M_PI / 2),
		M_PI / 2
	};

	GLuint _uni[2];

	Obj* _parent;

	static GLfloat _vtx[3];
	static GLushort _idx[1];

	static GLfloat _vtc[2 * 2][3];
	static GLushort _idc[2][3];

	static GLfloat _speed;

	enum {
		BED,
		OUTER,
		TAIL,
		DATA
	};
} Truck;

Truck* truckMk(Array* init, glm::vec3 loc = glm::vec3(0.0), glm::vec3 rot = glm::vec3(0.0));

void truckDel(Truck* truck);

void truckAnim(Truck* truck, glm::vec3 loc, glm::vec3 rot);

extern "C" void truckTurn(Truck* truck, float delta);

extern "C" void truckMv(Truck* truck, float delta);
