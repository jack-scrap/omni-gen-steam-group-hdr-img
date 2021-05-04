#include <cstdlib>
#include <iostream>
#include <glm/glm.hpp>

#include "cargo_ship.h"
#include "layout.h"
#include "arr.h"

CargoShip* cargoShipMk(char* init, unsigned int sz, glm::vec3 loc, glm::vec3 rot) {
	CargoShip* _ = (CargoShip*) malloc(sizeof (CargoShip));

	_->_data = arrMk(init, sz, "state", glm::vec3(-(layout::stride[Y] * sz), 1.0, layout::stride[X]));

	Obj* child[] = {
		_->_data->_parent
	};

	_->_parent = objMk("cargo_ship", "obj", "dir", true, child, sizeof child / sizeof *child, loc + glm::vec3(0.0, 10.0, 0.0), rot);

	glm::vec3 origin = _->_parent->_acc * glm::vec4(glm::vec3(0.0), 1.0);
	for (int i = 0; i < 3; i++) {
		_->_loc[i] = origin[i];
	}

	return _;
}

void cargoShipMv(CargoShip* cargoShip, float delta) {
	glm::vec3 loc = glm::vec3(delta, 0.0, 0.0);

	objAnim(cargoShip->_parent, loc, glm::vec3(0.0));

	glm::vec3 origin = cargoShip->_parent->_acc * glm::vec4(glm::vec3(0.0), 1.0);
	for (int i = 0; i < 3; i++) {
		cargoShip->_loc[i] = origin[i];
	}
}
