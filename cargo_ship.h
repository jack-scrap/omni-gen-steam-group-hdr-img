#pragma once

#include "obj.h"
#include "array.h"

typedef struct CargoShip {
	Obj* _parent;

	Array* data;

	float offset[3];

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

CargoShip* cargoShipMk(Array* array, glm::vec3 loc = glm::vec3(0.0), glm::vec3 rot = glm::vec3(0.0));

void cargoShipDel(CargoShip* inst);

void cargoShipAnim(CargoShip* inst, glm::vec3 loc);

extern "C" void cargoShipMv(CargoShip* inst, float delta);
