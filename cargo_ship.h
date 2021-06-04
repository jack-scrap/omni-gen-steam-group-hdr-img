#pragma once

#include "obj.h"
#include "arr.h"

typedef struct CargoShip {
	Arr* _data;
	GLfloat _loc[3];
	static constexpr unsigned int
		_x = 10,
		_y = 4;

	Obj* _parent;

	static GLfloat _speed;

	enum {
		BED,
		DATA
	};
} CargoShip;

CargoShip* cargoShipMk(char* init, glm::vec3 loc = glm::vec3(0.0), glm::vec3 rot = glm::vec3(0.0));

extern "C" void cargoShipMv(CargoShip* cargoShip, float delta);
