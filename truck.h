#pragma once

#include <glm/glm.hpp>

#include "obj.h"
#include "pt.h"
#include "arr.h"

typedef struct Truck {
	Arr* _data;

	GLfloat _loc[3];

	float _ang;

	static constexpr GLfloat _rngWheel[2] = {
		-(M_PI / 2),
		M_PI / 2
	};

	char* _ptr;
	unsigned int _no;

	Obj* _parent;

	GLfloat _vtx[3] = {
		0.0, 0.0, 0.0
	};

	GLushort _idx[1] = {
		0
	};

	GLuint _uniSzBed;
	GLuint _uniSzOuter;
} Truck;

Truck* truckMk(glm::vec3 loc = glm::vec3(0.0), glm::vec3 rot = glm::vec3(0.0));

Truck* truckMk(char* init, unsigned int no, glm::vec3 loc = glm::vec3(0.0), glm::vec3 rot = glm::vec3(0.0));

extern "C" void truckTurn(Truck* truck, float delta);

extern "C" void truckMv(Truck* truck, float delta);
