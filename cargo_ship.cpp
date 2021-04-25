#include <cstdlib>

#include "cargo_ship.h"

CargoShip* cargoShipMk(glm::vec3 loc, glm::vec3 rot) {
	CargoShip* _ = (CargoShip*) malloc(sizeof (CargoShip));

	_->_parent = objMk("cargo_ship", "obj", "dir", true, loc, rot);

	return _;
}
