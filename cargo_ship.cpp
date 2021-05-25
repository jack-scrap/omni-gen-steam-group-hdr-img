#include <cstdlib>
#include <iostream>
#include <glm/glm.hpp>

#include "cargo_ship.h"
#include "layout.h"
#include "arr.h"
#include "util.h"

CargoShip* cargoShipMk(char* init, glm::vec3 loc, glm::vec3 rot) {
	CargoShip* _ = (CargoShip*) malloc(sizeof (CargoShip));

	_->_data = arrMk(init, _->_x, _->_y, "state", glm::vec3(-(layout::stride[Y] * _->_x * _->_y), 1.0, layout::stride[X]));

	// layout
	glm::vec2 pad = {
		layout::margin * 2,
		layout::margin * 2
	};

	glm::vec2 sz = {
		(_->_x * layout::stride[Z]) + pad[X],
		(_->_y * layout::stride[X]) + pad[Y]
	};

	// data
	int i = 0;
	for (int y = 0; y < 2; y++) {
		for (int x = 0; x < 2; x++) {
			_->_vtc[i] = x * sz[X];
			_->_vtc[i + 1] = 0.0;
			_->_vtc[i + 2] = y * sz[Y];

			i += 3;
		}
	}

	std::vector<GLushort> seq;
	for (int i = 0; i < 2 * 2; i++) {
		seq.push_back(i);
	}

	std::vector<GLushort> idc = util::mesh::strip(seq);
	for (int i = 0; i < 2 * 2; i++) {
		_->_idc[i] = idc[i];
	}

	Obj* child[2];

	child[0] = objMk(_->_vtc, 2 * 2 * 3, _->_idc, 2 * 3, "obj", "solid", false, glm::vec3(-(sz[X] / 2), 0.0, -(sz[Y] / 2)));

	child[1] = _->_data->_parent;

	_->_parent = objMk("cargo_ship", "obj", "dir", true, child, sizeof child / sizeof *child, loc, rot);

	glm::vec3 offset = _->_parent->_acc * glm::vec4(glm::vec3(0.0), 1.0);
	for (int i = 0; i < 3; i++) {
		_->_loc[i] = offset[i];
	}

	return _;
}

void cargoShipMv(CargoShip* cargoShip, float delta) {
	glm::vec3 dest = glm::vec3(delta, 0.0, 0.0);

	objAnim(cargoShip->_parent, dest, glm::vec3(0.0));

	glm::vec3 offset = cargoShip->_parent->_acc * glm::vec4(glm::vec3(0.0), 1.0);
	for (int i = 0; i < 3; i++) {
		cargoShip->_loc[i] = offset[i];
	}
}
