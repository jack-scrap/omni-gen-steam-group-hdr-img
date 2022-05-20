#include <cstdlib>
#include <iostream>
#include <glm/glm.hpp>

#include "cargo_ship.h"
#include "layout.h"
#include "array.h"
#include "util.h"
#include "omni.h"

unsigned int CargoShip::_x = 10;
unsigned int CargoShip::_y = 4;

GLfloat CargoShip::_lightVtc[2 * 2 * 3] = {
	0.0, -0.5, -0.3,
	0.0, -0.5, 0.3,
	0.0, 0.5, -0.3,
	0.0, 0.5, 0.3
};

GLushort CargoShip::_lightIdc[2 * 3] = {
	0, 1, 2,
	2, 1, 3
};

CargoShip* cargoShipMk(Array* init, glm::vec3 loc, glm::vec3 rot) {
	CargoShip* _ = (CargoShip*) malloc(sizeof (CargoShip));

	// bed
	glm::vec2 stride = glm::vec2(layout::item(layout::scoped(layout::idx[X])), layout::item(layout::scoped(layout::idx[Z])));

	glm::vec2 bound = glm::vec2(layout::item(_->_x * stride[X]), layout::item(_->_y * stride[Y]));

	GLfloat vtc[2 * 2 * 3];
	int i = 0;
	for (int y = 0; y < 2; y++) {
		for (int x = 0; x < 2; x++) {
			vtc[i] = x * bound[X];
			vtc[i + 1] = 0.0;
			vtc[i + 2] = y * bound[Y];

			i += 3;
		}
	}

	GLushort idc[2][3];
	util::mesh::strip(idc);

	Obj* child[2 + (2 * 2)];

	glm::vec2 center = layout::center(bound);
	child[CargoShip::BED] = objMk((GLfloat*) vtc, (GLushort*) idc, 2 * 3, "obj", "dir", false, glm::vec3(-center[X], 0.0, -center[Y]));

	// data
	_->_data = init;

	child[CargoShip::DATA] = nullptr;

	if (_->_data) {
		child[CargoShip::DATA] = _->_data->_parent;
	}

	// light
	for (int z = 0; z < 2; z++) {
		for (int i = 0; i < 2; i++) {
			child[2 + (z * 2) + i] = objMk(CargoShip::_lightVtc, CargoShip::_lightIdc, 2 * 3, "obj", "alert", false, glm::vec3(-(30.0 + (layout::margin * 2)), 0.0, ((z ? 1 : -1) * 7.0) + ((i ? 1 : -1) * 0.6)));
		}
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

void cargoShipAnim(CargoShip* cargoShip, glm::vec3 loc) {
	for (int i = 0; i < 2; i++) {
		cargoShip->_parent->_child[2 + i]->_active = true;
	}

	objMv(cargoShip->_parent, nullptr, loc, glm::vec3(0.0), CargoShip::_speed);

	for (int i = 0; i < 2; i++) {
		cargoShip->_parent->_child[2 + i]->_active = false;
	}
}

void cargoShipMv(CargoShip* cargoShip, float delta) {
	if (delta >= 0.0) {
		glm::vec3 dest = glm::vec3(delta, 0.0, 0.0);

		cargoShipAnim(cargoShip, dest);

		/* objAnim(cargoShip->_parent, nullptr, dest, glm::vec3(0.0), CargoShip::_speed); */
	} else {
		omni::err(omni::ERR_MV_RNG, {
			"cargo ship"
		});
	}

	// offset
	glm::vec3 offset = cargoShip->_parent->_acc * glm::vec4(glm::vec3(0.0), 1.0);
	for (int a = 0; a < 3; a++) {
		cargoShip->_loc[a] = offset[a];
	}
}
