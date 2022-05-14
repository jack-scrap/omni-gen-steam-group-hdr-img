#pragma once

#include "obj.h"
#include "array.h"

typedef struct CargoShip {
	Array* _data;

	float _loc[3];

	Obj* _parent;

	static unsigned int _x;
	static unsigned int _y;

	static GLfloat _lightVtc[2 * 2 * 3];
	static GLushort _lightIdc[2 * 3];

	static constexpr float _speed = 0.1;

	enum {
		BED,

		DATA
	};
} CargoShip;

CargoShip* cargoShipMk(Array* init, glm::vec3 loc = glm::vec3(0.0), glm::vec3 rot = glm::vec3(0.0));

void cargoShipDel(CargoShip* cargoShip);

void cargoShipAnim(CargoShip* cargoShip, glm::vec3 loc);

extern "C" void cargoShipMv(CargoShip* cargoShip, float delta);
