#include <cstdlib>
#include <iostream>
#include <glm/glm.hpp>

#include "cargo_ship.h"

CargoShip* cargoShipMk(glm::vec3 loc, glm::vec3 rot) {
	CargoShip* _ = (CargoShip*) malloc(sizeof (CargoShip));

	_->_parent = objMk("cargo_ship", "obj", "dir", true, loc, rot);

	return _;
}

void cargoShipZoom(CargoShip* cargoShip, float delta) {
	glm::vec3 loc = glm::vec3(
		delta,
		0.0,
		0.0
	);

	objAnim(cargoShip->_parent, loc, glm::vec3(0.0));
}
