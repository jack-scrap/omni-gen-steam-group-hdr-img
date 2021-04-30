#include <cstdlib>
#include <thread>
#include <glm/gtc/matrix_transform.hpp>
#include <iostream>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/string_cast.hpp>

#include "crane.h"
#include "scn.h"
#include "math.h"
#include "state.h"
#include "omni.h"

Crane* craneMk(glm::vec3 loc, glm::vec3 rot) {
	Crane* _ = (Crane*) malloc(sizeof (Crane));

	_->_op = nullptr;

	for (int i = 0; i < 3; i++) {
		_->_loc[i] = loc[i];
	}

	// wheel
	Obj* child[(2 * 2 * 2 * 2) + 1 + (2 * 2) + 1];
	int i = 0;
	for (int z = 0; z < 2; z++) {
		for (int x = 0; x < 2; x++) {
			for (int j = 0; j < 2; j++) {
				for (int k = 0; k < 2; k++) {
					Obj* rim[2];
					for (int b = 0; b < 2; b++) {
						rim[b] = objMk("rim", "obj", "dir", true, glm::vec3(0.0), glm::vec3(0.0, b * M_PI, 0.0));
					}

					child[i] = objMk("wheel", "obj", "dir", false, rim, 2, glm::vec3(((x ? 1 : -1) * 3.0) + (j ? 1 : -1), 1.0, ((z ? 1 : -1) * 10.0) + ((k ? 1 : -1) * 0.6)));

					i++;
				}
			}
		}
	}

	// claw
	Obj* claw[] = {
		objMk("crane/claw", "obj", "dir", true)
	};

	child[2 * 2 * 2 * 2] = objMk("crane/head", "obj", "dir", true, claw, 1, glm::vec3(0.0, 13.8, 0.0));

	// light
	std::vector<GLfloat> rect = util::mesh::rect::pos(glm::vec2(0.6, 1.0), Y, true);

	std::vector<GLushort> rng;
	for (int i = 0; i < 2 * 2; i++) {
		rng.push_back(i);
	}
	std::vector<GLushort> strip = util::mesh::strip(rng);

	i = 0;
	for (int z = 0; z < 2; z++) {
		for (int x = 0; x < 2; x++) {
			child[(2 * 2 * 2 * 2) + 1 + i] = objMk(&rect[0], 2 * 2 * 3, &strip[0], 3 * 2, "obj", "alert", false, glm::vec3((x ? 1 : -1) * (6.0 + (layout::pad * 2)), 1.74, (z ? 1 : -1) * 10.0), glm::vec3(0.0, M_PI / 2, 0.0));

			i++;
		}
	}

	child[(2 * 2 * 2 * 2) + 1 + (2 * 2)] = nullptr;

	_->_parent = objMk("crane/body", "obj", "dir", true, child, sizeof child / sizeof *child, loc, rot);

	return _;
}

void craneAnim(Crane* crane, glm::vec3 loc) {
	for (int i = 0; i < 2 * 2; i++) {
		crane->_parent->_child[(2 * 2 * 2 * 2) + 1 + i]->_active = true;
	}

	objTrans(crane->_parent, loc, glm::vec3(0.0, 0.0, 0.0));

	for (int i = 0; i < 2 * 2; i++) {
		crane->_parent->_child[(2 * 2 * 2 * 2) + 1 + i]->_active = false;
	}
}

void craneZoom(Crane* crane, float delta) {
	glm::vec3 dest = glm::vec3(delta, 0.0, 0.0);
	craneAnim(crane, dest);
}

void cranePan(Crane* crane, float delta) {
	Obj* targ = crane->_parent->_child[2 * 2 * 2 * 2];

	glm::mat4 trans = glm::translate(glm::mat4(1.0), glm::vec3(0.0, 0.0, delta));
	glm::vec3 dest = glm::vec3(targ->_model * trans * glm::vec4(glm::vec3(0.0), 1.0));

	if (
		dest[Z] > crane->_rngHead[MIN] &&
		dest[Z] < crane->_rngHead[MAX]
	) {
		objTrans(targ, glm::vec3(0.0, 0.0, delta), glm::vec3(0.0, 0.0, 0.0));
	} else {
		omni::err("Cannot move crane head; translation exceeds range");
	}
}

void cranePed(Crane* crane, float delta) {
	Obj*& targ = crane->_parent->_child[2 * 2 * 2 * 2]->_child[0];

	glm::mat4 model = glm::translate(glm::mat4(1.0), glm::vec3(0.0, delta, 0.0));
	glm::vec3 dest = glm::vec3(targ->_acc * model * glm::vec4(glm::vec3(0.0), 1.0));

	if (
		dest[Y] > crane->_rngClaw[MIN] &&
		dest[Y] < crane->_rngClaw[MAX]
	) {
		glm::vec3 loc = glm::vec3(0.0, delta, 0.0);

		objTrans(targ, loc, glm::vec3(0.0));
	} else {
		omni::err("Cannot move crane claw; translation exceeds range");
	}
}

void craneGrab(Crane* crane) {
	Obj*& head = crane->_parent->_child[2 * 2 * 2 * 2];
	Obj*& claw = head->_child[0];

	Obj*& slot = crane->_parent->_child[(2 * 2 * 2 * 2) + 1 + (2 * 2)];

	if (!crane->_op) {
		for (int i = 0; i < data->_x; i++) {
			Cont*& byte = data->_data[i]->_data;

			if (byte) {
				if (util::phys::aabb(claw, data->_data[i]->_data->_parent)) {
					crane->_op = byte;
					slot = byte->_parent;

					slot->_model = glm::mat4(1.0);

					data->_parent->_child[i]->_child[0] = nullptr;
					data->_data[i]->_data = nullptr;

					break;
				}
			} else {
				omni::err("Cannot grab; no applicable data");
			}
		}
	} else {
		for (int i = 0; i < data->_x; i++) {
			Cont*& byte = data->_data[i]->_data;

			if (!byte) {
				if (util::phys::aabb(crane->_op->_parent, data->_data[i]->_parent)) {
					data->_data[i]->_data = crane->_op;
					data->_parent->_child[i]->_child[0] = crane->_parent->_child[(2 * 2 * 2 * 2) + 1 + (2 * 2)];

					data->_parent->_child[i]->_child[0]->_model = glm::translate(glm::mat4(1.0), glm::vec3((layout::idx[X] / 2) + (layout::pad * 2 * 2), 2.0 / 2, (layout::idx[Y] / 2) + (layout::pad * 2 * 2)));

					objAcc(data->_parent, glm::mat4(1.0));

					crane->_op = nullptr;
					crane->_parent->_child[(2 * 2 * 2 * 2) + 1 + (2 * 2)] = nullptr;

					break;
				}
			}
		}
	}

	objAcc(crane->_parent, glm::mat4(1.0));
}
