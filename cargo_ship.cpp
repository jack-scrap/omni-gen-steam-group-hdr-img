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

CargoShip* cargoShipMk(Array* array, glm::vec3 loc, glm::vec3 rot) {
	CargoShip* inst = (CargoShip*) malloc(sizeof (CargoShip));

	// bed
	glm::vec2 stride = glm::vec2(layout::item(layout::scoped(layout::idx[X])), layout::item(layout::scoped(layout::idx[Z])));

	glm::vec2 bound = glm::vec2(layout::item(inst->_x * stride[X]), layout::item(inst->_y * stride[Y]));

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
	child[CargoShip::BED] = objMk(vtc, (GLushort*) idc, 2 * 3, Mesh::OBJ, "obj", "dir", false, glm::vec3(-center[X], 0.0, -center[Y]));

	// data
	inst->data = array;

	child[CargoShip::DATA] = nullptr;

	if (inst->data) {
		child[CargoShip::DATA] = inst->data->_parent;
	}

	// light
	for (int z = 0; z < 2; z++) {
		for (int i = 0; i < 2; i++) {
			child[2 + (z * 2) + i] = objMk(CargoShip::_lightVtc, CargoShip::_lightIdc, 2 * 3, Mesh::OBJ, "obj", "alert", false, glm::vec3(-(30.0 + (layout::margin * 2)), 0.0, ((z ? 1 : -1) * 7.0) + ((i ? 1 : -1) * 0.6)));
		}
	}

	inst->_parent = objMk("cargo_ship", "obj", "dir", true, child, sizeof child / sizeof *child, loc, rot);

	// offset
	glm::vec3 offset = util::matr::apply(glm::vec3(0.0), inst->_parent->_acc);
	for (int a = 0; a < 3; a++) {
		inst->offset[a] = offset[a];
	}

	return inst;
}

void cargoShipDel(CargoShip* inst) {
	arrayDel(inst->data);

	objDel(inst->_parent);

	free(inst);
}

void cargoShipAnim(CargoShip* inst, glm::vec3 loc) {
	for (int i = 0; i < 2 * 2; i++) {
		inst->_parent->_child[2 + i]->_active = true;
	}

	objMv(inst->_parent, nullptr, loc, glm::vec3(0.0), CargoShip::_speed);

	for (int i = 0; i < 2 * 2; i++) {
		inst->_parent->_child[2 + i]->_active = false;
	}
}

void cargoShipMv(CargoShip* inst, float delta) {
	if (delta >= 0.0) {
		glm::vec3 dest = glm::vec3(delta, 0.0, 0.0);

		cargoShipAnim(inst, dest);
	} else {
		omni::err(omni::ERR_MV_RNG, {
			"cargo ship"
		});
	}

	// offset
	glm::vec3 offset = util::matr::apply(glm::vec3(0.0), inst->_parent->_acc);
	for (int a = 0; a < 3; a++) {
		inst->offset[a] = offset[a];
	}
}
