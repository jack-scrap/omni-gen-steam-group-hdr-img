#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <thread>

#include "truck.h"
#include "scn.h"
#include "state.h"
#include "omni.h"

GLfloat Truck::_vtx[3] = {
	0.0, 0.0, 0.0
};

GLushort Truck::_idx[1] = {
	0
};

GLfloat Truck::_speed = 0.3;

Truck* truckMk(Arr* init, glm::vec3 loc, glm::vec3 rot) {
	Truck* _ = (Truck*) malloc(sizeof (Truck));

	_->_ang = 0.0;

	Obj* child[4 + (init->_x * 2 * 2) + 2];

	// bed
	Obj* bed = ptMk(Truck::_vtx, Truck::_idx, 1, "main", "bed", "dir", false, glm::vec3(0.0), glm::vec3(0.0, M_PI / 2, 0.0));

	bed->_uni = (GLint*) realloc(bed->_uni, (5 + 1) * sizeof (GLint));

	bed->_prog.use();

	bed->_uni[5] = glGetUniformLocation(bed->_prog._id, "sz");
	glUniform1ui(bed->_uni[5], init->_x);

	bed->_prog.unUse();

	Obj* outer = ptMk(Truck::_vtx, Truck::_idx, 1, "main", "outer", "dir", true, glm::vec3(0.0), glm::vec3(0.0, M_PI / 2, 0.0));

	outer->_uni = (GLint*) realloc(outer->_uni, (5 + 1) * sizeof (GLint));

	outer->_prog.use();

	outer->_uni[5] = glGetUniformLocation(outer->_prog._id, "sz");
	glUniform1ui(outer->_uni[5], init->_x);

	outer->_prog.unUse();

	child[Truck::BED] = bed;
	child[Truck::OUTER] = outer;

	child[Truck::TAIL] = objMk("truck/tail", "obj", "dir", true, glm::vec3(-(init->_x * layout::idx[Z]) - (layout::stroke * 2), -layout::stroke, 0.0), rot);

	// data
	_->_data = init;

	child[Truck::DATA] = _->_data->_parent;

	// wheel
	int i = 0;
	for (int z = 0; z < 2; z++) {
		for (int x = 0; x < init->_x * 2; x++) {
			Obj* rim[] = {
				objMk("rim", "obj", "dir", true)
			};

			child[4 + i] = objMk("wheel", "obj", "dir", false, rim, sizeof rim / sizeof *rim, glm::vec3(x * -2.6, -1, z ? 1 : -1), glm::vec3(0.0, z * M_PI, 0.0));

			i++;
		}
	}

	// light
	GLfloat vtc[2 * 2][3] = {
		{
			0.0, -0.3, -0.5
		}, {
			0.0, -0.3, 0.5
		}, {
			0.0, 0.3, -0.5
		}, {
			0.0, 0.3, 0.5
		}
	};

	GLushort idc[2][3];
	util::mesh::strip(idc);

	for (int z = 0; z < 2; z++) {
		child[4 + (init->_x * 2 * 2) + z] = objMk((GLfloat*) vtc, (GLushort*) idc, 2 * 3, "obj", "alert", false, glm::vec3(-(init->_x * layout::idx[Z]) - (layout::stroke * 2) - layout::stroke - layout::stroke, 0.0, z ? 1 : -1));
	}

	_->_parent = objMk("truck/front", "obj", "dir", true, child, sizeof child / sizeof *child, loc + glm::vec3(2.4, 1.3, 0.0), rot);

	// offset
	glm::vec3 offset = _->_parent->_acc * glm::vec4(glm::vec3(0.0), 1.0);
	for (int i = 0; i < 3; i++) {
		_->_loc[i] = offset[i];
	}

	return _;
}

void truckAnim(Truck* truck, glm::vec3 loc, glm::vec3 rot) {
	for (int i = 0; i < 2; i++) {
		truck->_parent->_child[4 + (truck->_data->_x * 2 * 2) + i]->_active = true;
	}

	objTrans(truck->_parent, loc, rot);

	for (int i = 0; i < 2; i++) {
		truck->_parent->_child[4 + (truck->_data->_x * 2 * 2) + i]->_active = false;
	}
}

void truckMv(Truck* truck, float delta) {
	glm::vec3 dest = glm::vec3(delta, 0.0, 0.0);

	truckAnim(truck, dest, glm::vec3(0.0));

	glm::vec3 offset = truck->_parent->_acc * glm::vec4(glm::vec3(0.0), 1.0);
	for (int i = 0; i < 3; i++) {
		truck->_loc[i] = offset[i];
	}
}

void truckTurn(Truck* truck, float delta) {
	if (
		truck->_ang + delta > truck->_rngWheel[MIN] &&
		truck->_ang + delta < truck->_rngWheel[MAX]
	) {
		truck->_ang += delta;

		for (int z = 0; z < 2; z++) {
			objAnim(truck->_parent->_child[z * 2], glm::vec3(0.0), glm::vec3(0.0, delta, 0.0));
		}
	} else {
		omni::err("Cannot turn truck; rotation of wheels exceeds range");
	}

	// offset
	glm::vec3 offset = truck->_parent->_acc * glm::vec4(glm::vec3(0.0), 1.0);
	for (int i = 0; i < 3; i++) {
		truck->_loc[i] = offset[i];
	}
}
