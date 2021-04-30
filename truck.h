#pragma once

#include <glm/glm.hpp>

#include "obj.h"
#include "pt.h"

typedef struct Truck {
	GLfloat _loc[3];

	float _ang;

	char* _ptr;
	unsigned int _sz;

	Obj* _parent;

	static constexpr GLfloat _rngWheel[2] = {
		-(M_PI / 2),
		M_PI / 2
	};
} Truck;

Truck* truckMk(glm::vec3 loc = glm::vec3(0.0), glm::vec3 rot = glm::vec3(0.0, 0.0, 0.0));

Truck* truckMk(char* data, unsigned int sz, glm::vec3 loc = glm::vec3(0.0), glm::vec3 rot = glm::vec3(0.0, 0.0, 0.0));

extern "C" void truckTurn(Truck* truck, float delta);

extern "C" void truckZoom(Truck* truck, float delta);
