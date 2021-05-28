#pragma once

#include "obj.h"
#include "arr.h"

typedef struct CargoShip {
	Arr* _data;
	GLfloat _loc[3];
	static constexpr unsigned int
		_x = 6,
		_y = 2;

	GLfloat _vtc[2 * 2 * 3];
	GLushort _idc[2 * 2];

	GLfloat _speed = 0.1;

	Obj* _parent;
} CargoShip;

CargoShip* cargoShipMk(char* init, glm::vec3 loc = glm::vec3(0.0), glm::vec3 rot = glm::vec3(0.0));

extern "C" void cargoShipMv(CargoShip* cargoShip, float delta);
