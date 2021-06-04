#include <cstdlib>
#include <iostream>
#include <glm/glm.hpp>

#include "cargo_ship.h"
#include "layout.h"
#include "array.h"
#include "util.h"

GLfloat CargoShip::_speed = 0.1;

CargoShip* cargoShipMk(char* init, glm::vec3 loc, glm::vec3 rot) {
	CargoShip* _ = (CargoShip*) malloc(sizeof (CargoShip));

	// bed
	glm::vec2 sz = layout::sz({
		_->_x,
		_->_y
	});

	GLfloat vtc[2 * 2][3];
	for (int y = 0; y < 2; y++) {
		for (int x = 0; x < 2; x++) {
			vtc[(y * 2) + x][0] = x * sz[X];
			vtc[(y * 2) + x][1] = 0.0;
			vtc[(y * 2) + x][2] = y * sz[Y];
		}
	}

	GLushort idc[2][3];
	util::mesh::strip(idc);

	Obj* child[2];

	child[CargoShip::BED] = objMk((GLfloat*) vtc, (GLushort*) idc, 2 * 3, "obj", "dir", false, glm::vec3(-(sz[X] / 2), 0.0, -(sz[Y] / 2)));

	// data
	_->_data = arrMk(init, _->_x, _->_y, "state", glm::vec3(-(layout::stride[Y] * _->_x * _->_y), 1.0, layout::stride[X]));

	child[CargoShip::DATA] = _->_data->_parent;

	_->_parent = objMk("cargo_ship", "obj", "dir", true, child, sizeof child / sizeof *child, loc, rot);

	// offset
	glm::vec3 offset = _->_parent->_acc * glm::vec4(glm::vec3(0.0), 1.0);
	for (int i = 0; i < 3; i++) {
		_->_loc[i] = offset[i];
	}

	return _;
}

void cargoShipMv(CargoShip* cargoShip, float delta) {
	glm::vec3 dest = glm::vec3(delta, 0.0, 0.0);

	objAnim(cargoShip->_parent, dest, glm::vec3(0.0));

	// offset
	glm::vec3 offset = cargoShip->_parent->_acc * glm::vec4(glm::vec3(0.0), 1.0);
	for (int i = 0; i < 3; i++) {
		cargoShip->_loc[i] = offset[i];
	}
}
