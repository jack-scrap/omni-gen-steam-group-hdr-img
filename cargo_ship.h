#pragma once

#include "obj.h"
#include "arr.h"

typedef struct CargoShip {
	Arr* _data;
	static constexpr unsigned int _x = 6;
	static constexpr unsigned int _y = 2;

	GLfloat _loc[3];

	Obj* _parent;
} CargoShip;

CargoShip* cargoShipMk(char* init, unsigned int no, glm::vec3 loc = glm::vec3(0.0), glm::vec3 rot = glm::vec3(0.0, 0.0, 0.0));

extern "C" void cargoShipMv(CargoShip* cargoShip, float delta);
