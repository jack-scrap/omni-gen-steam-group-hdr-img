#include <cstdlib>
#include <iostream>
#include <glm/glm.hpp>

#include "cargo_ship.h"
#include "layout.h"
#include "data.h"

CargoShip* cargoShipMk(char* data, unsigned int sz, glm::vec3 loc, glm::vec3 rot) {
	CargoShip* _ = (CargoShip*) malloc(sizeof (CargoShip));

	Data* data = dataMk(data, sz, "state", glm::vec3(-(layout::stride[Y] * sz), 1.0, layout::stride[X]));

	Obj* child[] = {
		data->_parent
	};

	_->_parent = objMk("cargo_ship", "obj", "dir", true, child, sizeof child / sizeof *child, loc + glm::vec3(0.0, 10.0, 0.0), rot);

	return _;
}

void cargoShipZoom(CargoShip* cargoShip, float delta) {
	glm::vec3 loc = glm::vec3(delta, 0.0, 0.0);

	objAnim(cargoShip->_parent, loc, glm::vec3(0.0));
}
