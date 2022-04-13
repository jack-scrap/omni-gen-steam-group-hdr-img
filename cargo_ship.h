#pragma once

#include "obj.h"
#include "array.h"

typedef struct CargoShip {
	Array* _data;

	float _loc[3];

	Obj* _parent;

	static unsigned int _x;
	static unsigned int _y;

	static float _speed;

	enum {
		BED,

		DATA
	};
} CargoShip;

CargoShip* cargoShipMk(Array* init, glm::vec3 loc = glm::vec3(0.0), glm::vec3 rot = glm::vec3(0.0));

void cargoShipDel(CargoShip* cargoShip);

extern "C" void cargoShipMv(CargoShip* cargoShip, float delta);
