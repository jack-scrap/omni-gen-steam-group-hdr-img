#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <thread>

#include "truck.h"
#include "scn.h"
#include "state.h"
#include "omni.h"
#include "util.h"
#include "layout.h"

GLfloat Truck::_bedVtx[3] = {
	0.0, 0.0, 0.0
};

GLushort Truck::_bedIdx[1] = {
	0
};

GLfloat Truck::_lightVtc[2 * 2 * 3] = {
	0.0, -0.3, -1.0,
	0.0, -0.3, 0.0,
	0.0, 0.3, 0.0,
	0.0, 0.3, 1.0
};

GLushort Truck::_lightIdc[2 * 3] = {
	0, 1, 2,
	2, 1, 3
};

Truck* truckMk(Array* init, glm::vec3 loc, glm::vec3 rot) {
	Truck* inst = (Truck*) malloc(sizeof (Truck));

	inst->ang = 0.0;

	Obj* child[4 + ((init->_x + 1) * 2) + 2 + init->_x];

	// bed
	Obj* bed = ptMk(Truck::_bedVtx, Truck::_bedIdx, 1, "main", "bed", "dir", false, glm::vec3(0.0), glm::vec3(0.0, M_PI / 2, 0.0));

	bed->_prog.use();

	bed->_uni = (GLint*) realloc(bed->_uni, (5 + 1) * sizeof (GLint));

	bed->_uni[5] = glGetUniformLocation(bed->_prog._id, "sz");
	glUniform1ui(bed->_uni[5], init->_x);

	bed->_prog.unUse();

	Obj* outer = ptMk(Truck::_bedVtx, Truck::_bedIdx, 1, "main", "outer", "dir", true, glm::vec3(0.0), glm::vec3(0.0, M_PI / 2, 0.0));

	outer->_prog.use();

	outer->_uni = (GLint*) realloc(outer->_uni, (5 + 1) * sizeof (GLint));

	outer->_uni[5] = glGetUniformLocation(outer->_prog._id, "sz");
	glUniform1ui(outer->_uni[5], init->_x);

	outer->_prog.unUse();

	child[Truck::BED] = bed;
	child[Truck::OUTER] = outer;

	GLfloat ht = -(0.2 * 2);

	child[Truck::TAIL] = objMk("truck/tail", "obj", "dir", true, glm::vec3(-layout::scoped(init->_x * layout::scoped(layout::idx[Z]) - (0.32 / 2)), -(0.2 * 2), 0.0));

	// data
	inst->data = init;

	child[Truck::DATA] = inst->data->_parent;

	// wheel
	glm::vec3 overhead = glm::vec3(-((layout::margin * 2) + (layout::stroke / 2)), 0.0, 0.0);

	int w = 0;
	for (int z = 0; z < 2; z++) {
		for (int x = 0; x < init->_x + 1; x++) {
			Obj* wheel = objMk("wheel", "obj", "dir", false);

			child[4 + w] = objMk("rim", "obj", "dir", true, &wheel, 1, overhead + glm::vec3(x * -layout::scoped(layout::idx[Z]), -1.0 + (ht * 2) + ht, (z ? 1 : -1) * (layout::scoped(layout::idx[X]) / 2)), glm::vec3(0.0, z * M_PI, 0.0));

			w++;
		}
	}

	// light
	for (int z = 0; z < 2; z++) {
		child[4 + ((init->_x + 1) * 2) + z] = objMk(Truck::_lightVtc, Truck::_lightIdc, 2 * 3, Mesh::OBJ, "obj", "alert", false, glm::vec3(-(layout::scoped(init->_x * layout::scoped(layout::idx[Z])) + (layout::margin * 2)), -0.2, (z ? 1 : -1) * 0.74), glm::vec3(0.0, z * M_PI, 0.0));
	}

	// wheel-well
	for (int i = 0; i < init->_x; i++) {
		child[4 + ((init->_x + 1) * 2) + 2 + i] = objMk("truck/well", "obj", "dir", true, glm::vec3(-((layout::scoped(layout::idx[Z]) / 2) + (i * layout::scoped(layout::idx[Z]))), ht, 0.0));
	}

	inst->_parent = objMk("truck/front", "obj", "dir", true, child, sizeof child / sizeof *child, loc + glm::vec3(2.4, 1.0 + 1.3, 0.0), rot);

	// offset
	glm::vec3 offset = inst->_parent->_acc * glm::vec4(glm::vec3(0.0), 1.0);
	for (int a = 0; a < 3; a++) {
		inst->offset[a] = offset[a];
	}

	return inst;
}

void truckDel(Truck* inst) {
	arrayDel(inst->data);

	objDel(inst->_parent);

	free(inst);
}

void truckAnim(Truck* inst, glm::vec3 loc, glm::vec3 rot) {
	for (int i = 0; i < 2; i++) {
		inst->_parent->_child[4 + ((inst->data->_x + 1) * 2) + i]->_active = true;
	}

	objMv(inst->_parent, nullptr, loc, rot, Truck::_speed);

	for (int i = 0; i < 2; i++) {
		inst->_parent->_child[4 + ((inst->data->_x + 1) * 2) + i]->_active = false;
	}
}

void truckMv(Truck* inst, float delta) {
	glm::vec3 dest = glm::vec3(delta, 0.0, 0.0);

	truckAnim(inst, dest, glm::vec3(0.0));

	glm::vec3 offset = inst->_parent->_acc * glm::vec4(glm::vec3(0.0), 1.0);
	for (int a = 0; a < 3; a++) {
		inst->offset[a] = offset[a];
	}
}

void truckTurn(Truck* inst, float delta) {
	if (inst->ang + delta > inst->_rngWheel[MIN] && inst->ang + delta < inst->_rngWheel[MAX]) {
		inst->ang += delta;

		for (int z = 0; z < 2; z++) {
			objAnim(inst->_parent->_child[4 + z], inst->_parent, glm::vec3(0.0), glm::vec3(0.0, glm::radians(delta), 0.0));
		}

		objAnim(inst->_parent, nullptr, glm::vec3(0.0), glm::vec3(0.0, glm::radians(delta), 0.0));
	} else {
		omni::err(omni::ERR_MV_RNG, {
			"truck wheels"
		});
	}

	// offset
	glm::vec3 offset = inst->_parent->_acc * glm::vec4(glm::vec3(0.0), 1.0);
	for (int a = 0; a < 3; a++) {
		inst->offset[a] = offset[a];
	}
}
