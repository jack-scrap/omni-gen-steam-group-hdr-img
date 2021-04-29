#pragma once

#include "obj.h"

typedef struct {
	Obj* _parent;
} CargoShip;

CargoShip* cargoShipMk(char* init, unsigned int sz, glm::vec3 loc = glm::vec3(0.0), glm::vec3 rot = glm::vec3(0.0, 0.0, 0.0));

extern "C" void cargoShipZoom(CargoShip* cargoShip, float delta);
