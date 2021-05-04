#pragma once

#include <glm/glm.hpp>

#include "obj.h"
#include "pt.h"
#include "data.h"

typedef struct Truck {
	Arr* _data;

	GLfloat _loc[3];

	float _ang;

	char* _ptr;
	unsigned int _sz;

	Obj* _parent;

	GLfloat _vtx[3] = {
		0.0, 0.0, 0.0
	};

	GLushort _idx[1] = {
		0
	};

	static constexpr GLfloat _rngWheel[2] = {
		-(M_PI / 2),
		M_PI / 2
	};
} Truck;

Truck* truckMk(glm::vec3 loc = glm::vec3(0.0), glm::vec3 rot = glm::vec3(0.0, 0.0, 0.0));

Truck* truckMk(char* data, unsigned int sz, glm::vec3 loc = glm::vec3(0.0), glm::vec3 rot = glm::vec3(0.0, 0.0, 0.0));

extern "C" void truckTurn(Truck* truck, float delta);

extern "C" void truckMv(Truck* truck, float delta);
