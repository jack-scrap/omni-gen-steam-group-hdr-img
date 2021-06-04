#pragma once

#include "obj.h"
#include "array.h"

typedef struct CargoShip {
	Arr* _data;
	GLfloat _loc[3];

	Obj* _parent;

	static unsigned int
		_x,
		_y;

	static GLfloat _speed;

	enum {
		BED,
		DATA
	};
} CargoShip;

CargoShip* cargoShipMk(Arr* init, glm::vec3 loc = glm::vec3(0.0), glm::vec3 rot = glm::vec3(0.0));

extern "C" void cargoShipMv(CargoShip* cargoShip, float delta);
