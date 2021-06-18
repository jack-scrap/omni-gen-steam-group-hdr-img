#define GLM_ENABLE_EXPERIMENTAL

#include <cstdlib>
#include <thread>
#include <glm/gtc/matrix_transform.hpp>
#include <iostream>
#include <glm/gtx/string_cast.hpp>

#include "crane.h"
#include "scn.h"
#include "math.h"
#include "state.h"
#include "omni.h"

GLfloat Crane::_speed = 0.2;

Crane* craneMk(Cont* init, glm::vec3 loc, glm::vec3 rot) {
	Crane* _ = (Crane*) malloc(sizeof (Crane));

	if (init) {
		_->_data = init;
	} else {
		_->_data = nullptr;
	}

	Obj* child[1 + 1 + (2 * 2 * 2 * 2) + (2 * 2)];

	// head
	Obj* claw[] = {
		objMk("crane/claw", "obj", "dir", true)
	};

	child[Crane::HEAD] = objMk("crane/head", "obj", "dir", true, glm::vec3(0.0, 13.8, 0.0));

	// data
	if (_->_data) {
		child[Crane::SLOT] = _->_data->_parent;
	} else {
		child[Crane::SLOT] = nullptr;
	}

	// wheel
	int w = 0;
	for (int z = 0; z < 2; z++) {
		for (int x = 0; x < 2; x++) {
			for (int j = 0; j < 2; j++) {
				for (int k = 0; k < 2; k++) {
					Obj* rim[2];
					for (int b = 0; b < 2; b++) {
						rim[b] = objMk("rim", "obj", "dir", true, glm::vec3(0.0), glm::vec3(0.0, b * M_PI, 0.0));
					}

					child[1 + 1 + w] = objMk("wheel", "obj", "dir", false, rim, 2, glm::vec3(-0.5 + ((x ? 1 : -1) * 3.0) + (j ? 1 : -1), 1.0, ((z ? 1 : -1) * 10.0) + ((k ? 1 : -1) * 0.6)));

					w++;
				}
			}
		}
	}

	// light
	std::vector<GLfloat> rect = util::mesh::rect::pos(glm::vec2(0.6, 1.0), Y, true);

	GLushort idc[2][3];
	util::mesh::strip(idc);

	int l = 0;
	for (int z = 0; z < 2; z++) {
		for (int x = 0; x < 2; x++) {
			child[1 + 1 + (2 * 2 * 2 * 2) + l] = objMk(&rect[0], (GLushort*) idc, 3 * 2, "obj", "alert", false, glm::vec3((x ? 1 : -1) * (6.0 + (layout::pad * 2)), 1.74, (z ? 1 : -1) * 10.0), glm::vec3(0.0, M_PI / 2, 0.0));

			l++;
		}
	}

	_->_parent = objMk("crane/body", "obj", "dir", true, child, sizeof child / sizeof *child, loc, rot);

	// offset
	glm::vec3 offset = _->_parent->_acc * glm::vec4(glm::vec3(0.0), 1.0);
	for (int a = 0; a < 3; a++) {
		_->_loc[a] = offset[a];
	}

	return _;
}

void craneAnim(Crane* crane, glm::vec3 loc) {
	for (int i = 0; i < 2 * 2; i++) {
		crane->_parent->_child[1 + 1 + (2 * 2 * 2 * 2) + i]->_active = true;
	}

	objMv(crane->_parent, loc, glm::vec3(0.0));

	for (int i = 0; i < 2 * 2; i++) {
		crane->_parent->_child[1 + 1 + (2 * 2 * 2 * 2) + i]->_active = false;
	}
}

void craneZoom(Crane* crane, float delta) {
	glm::vec3 dest = glm::vec3(delta, 0.0, 0.0);
	craneAnim(crane, dest);

	glm::vec3 offset = crane->_parent->_acc * glm::vec4(glm::vec3(0.0), 1.0);
	for (int a = 0; a < 3; a++) {
		crane->_loc[a] = offset[a];
	}
}

void cranePan(Crane* crane, float delta) {
	glm::mat4 trans = glm::translate(glm::mat4(1.0), glm::vec3(0.0, 0.0, delta));
	glm::vec3 dest = glm::vec3(crane->_parent->_child[Crane::HEAD]->_model * trans * glm::vec4(glm::vec3(0.0), 1.0));

	if (
		dest[Z] > crane->_rngHead[MIN] &&
		dest[Z] < crane->_rngHead[MAX]
	) {
		objMv(crane->_parent->_child[Crane::HEAD], glm::vec3(0.0, 0.0, delta), glm::vec3(0.0));
	} else {
		omni::err("Cannot move crane head; translation exceeds range");
	}

	glm::vec3 offset = crane->_parent->_acc * glm::vec4(glm::vec3(0.0), 1.0);
	for (int a = 0; a < 3; a++) {
		crane->_loc[a] = offset[a];
	}
}

void cranePed(Crane* crane, float delta) {
	glm::mat4 trans = glm::translate(glm::mat4(1.0), glm::vec3(0.0, delta, 0.0));
	glm::vec3 dest = glm::vec3(crane->_parent->_child[Crane::HEAD]->_acc * trans * glm::vec4(glm::vec3(0.0), 1.0));

	if (
		dest[Y] > crane->_rngClaw[MIN] &&
		dest[Y] < crane->_rngClaw[MAX]
	) {
		glm::vec3 offset = glm::vec3(0.0, delta, 0.0);

		objMv(crane->_parent->_child[Crane::HEAD]->_child[Crane::CLAW], offset, glm::vec3(0.0));
	} else {
		omni::err("Cannot move crane claw; translation exceeds range");
	}

	glm::vec3 offset = crane->_parent->_acc * glm::vec4(glm::vec3(0.0), 1.0);
	for (int a = 0; a < 3; a++) {
		crane->_loc[a] = offset[a];
	}
}

void craneGrab(Crane* crane) {
	for (int i = 0; i < noData; i++) {
		switch (type[i]) {
			case omni::SCALAR: {
				Idx* idx = (Idx*) data[i]->_ptr;

				if (idx->_data) {
					if (util::phys::coll(crane->_parent->_child[Crane::HEAD]->_child[Crane::CLAW]->_aabb, idx->_data->_parent->_aabb)) {
						craneInsert(crane, idxPop(idx));

						return;
					}
				}

				break;
			}

			case omni::ARRAY: {
				Array* array = (Array*) data[i]->_ptr;

				for (int i = 0; i < array->_x * array->_y; i++) {
					Idx* idx = array->_data[i];

					if (idx->_data) {
						if (util::phys::coll(crane->_parent->_child[Crane::HEAD]->_child[Crane::CLAW]->_aabb, idx->_data->_parent->_aabb)) {
							craneInsert(crane, idxPop(idx));

							return;
						}
					}
				}

				break;
			}
		}
	}
}

void craneInsert(Crane* crane, Cont* byte) {
	crane->_data = byte;
	crane->_parent->_child[Crane::SLOT] = crane->_data->_parent;

	objAcc(crane->_parent, glm::mat4(1.0));
}
