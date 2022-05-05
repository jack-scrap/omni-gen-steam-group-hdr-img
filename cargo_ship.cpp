#include <cstdlib>
#include <iostream>
#include <glm/glm.hpp>

#include "cargo_ship.h"
#include "layout.h"
#include "array.h"
#include "util.h"

unsigned int CargoShip::_x = 10;
unsigned int CargoShip::_y = 4;

CargoShip* cargoShipMk(Array* init, glm::vec3 loc, glm::vec3 rot) {
	CargoShip* _ = (CargoShip*) malloc(sizeof (CargoShip));

	// bed
	glm::vec2 stride = glm::vec2(layout::item(layout::bordered(layout::idx[X])), layout::item(layout::bordered(layout::idx[Z])));

	glm::vec2 bound = glm::vec2(layout::item(_->_x * stride[X]), layout::item(_->_y * stride[Y]));

	GLfloat vtc[2 * 2][3];
	for (int y = 0; y < 2; y++) {
		for (int x = 0; x < 2; x++) {
			vtc[(y * 2) + x][0] = x * bound[X];
			vtc[(y * 2) + x][1] = 0.0;
			vtc[(y * 2) + x][2] = y * bound[Y];
		}
	}

	GLushort idc[2][3];
	util::mesh::strip(idc);

	Obj* child[2];

	glm::vec2 center = layout::center(bound);
	child[CargoShip::BED] = objMk((GLfloat*) vtc, (GLushort*) idc, 2 * 3, "obj", "dir", false, glm::vec3(-center[X], 0.0, -center[Y]));

	// data
	_->_data = init;

	child[CargoShip::DATA] = nullptr;

	if (_->_data) {
		child[CargoShip::DATA] = _->_data->_parent;
	}

	_->_parent = objMk("cargo_ship", "obj", "dir", true, child, sizeof child / sizeof *child, loc, rot);

	// offset
	glm::vec3 offset = _->_parent->_acc * glm::vec4(glm::vec3(0.0), 1.0);
	for (int a = 0; a < 3; a++) {
		_->_loc[a] = offset[a];
	}

	return _;
}

void cargoShipDel(CargoShip* cargoShip) {
	arrayDel(cargoShip->_data);

	objDel(cargoShip->_parent);

	free(cargoShip);
}

void cargoShipMv(CargoShip* cargoShip, float delta) {
	glm::vec3 dest = glm::vec3(delta, 0.0, 0.0);

	objAnim(cargoShip->_parent, nullptr, dest, glm::vec3(0.0), CargoShip::_speed);

	// offset
	glm::vec3 offset = cargoShip->_parent->_acc * glm::vec4(glm::vec3(0.0), 1.0);
	for (int a = 0; a < 3; a++) {
		cargoShip->_loc[a] = offset[a];
	}
}
