#include <cstdlib>
#include <glm/gtc/matrix_transform.hpp>
#include <thread>
#include <iostream>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/string_cast.hpp>

#include "crane.h"
#include "scn.h"
#include "math.h"
#include "state.h"

Crane* craneMk(glm::vec3 loc, glm::vec3 rot) {
	Crane* _ = (Crane*) malloc(sizeof (Crane));

	// wheel
	Obj* child[(2 * 2 * 2 * 2) + 1 + (2 * 2)];
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
	std::vector<GLfloat> quad = util::mesh::quad::pos(glm::vec2(0.6, 1.0));

	std::vector<GLushort> rng;
	for (int i = 0; i < 2 * 2; i++) {
		rng.push_back(i);
	}
	std::vector<GLushort> strip = util::mesh::strip(rng);

	i = 0;
	for (int z = 0; z < 2; z++) {
		for (int x = 0; x < 2; x++) {
			child[(2 * 2 * 2 * 2) + 1 + i] = objMk(&quad[0], 2 * 2 * 3, &strip[0], 3 * 2, "obj", "alert", false, glm::vec3((x ? 1 : -1) * (6.0 + (state::pad * 2)), 1.74, (z ? 1 : -1) * 10.0), glm::vec3(0.0, M_PI / 2, 0.0));

			i++;
		}
	}

	_->_parent = objMk("crane/body", "obj", "dir", true, child, sizeof child / sizeof *child, loc, rot);

	return _;
}

void craneAnim(Crane* crane, glm::vec3 loc) {
	for (int i = 0; i < 2 * 2; i++) {
		crane->_parent->_child[(2 * 2 * 2 * 2) + 1 + i]->_active = true;
	}

	std::thread t(objAnim, crane->_parent, glm::vec3(1.0, 0.0, 0.0), glm::vec3(0.0, 0.0, 0.0));
	t.join();

	for (int i = 0; i < 2 * 2; i++) {
		crane->_parent->_child[(2 * 2 * 2 * 2) + 1 + i]->_active = false;
	}
}

void craneZoom(Crane* crane, bool dir) {
	glm::vec3 loc = glm::vec3(
		dir ? 1 : -1,
		0.0,
		0.0
	);
	std::thread t(craneAnim, crane, loc);
	t.detach();
}

void cranePan(Crane* crane, bool dir) {
	Obj* targ = crane->_parent->_child[2 * 2 * 2 * 2];

	float inc = dir ? 1 : -1;

	glm::mat4 d = glm::translate(glm::mat4(1.0), glm::vec3(0.0, 0.0, inc));
	glm::vec3 loc = glm::vec3(targ->_model * d * glm::vec4(glm::vec3(0.0), 1.0));

	if (
		loc[Z] > crane->_rngHead[MIN] &&
		loc[Z] < crane->_rngHead[MAX]
	) {
		objTrans(targ, glm::vec3(0.0, 0.0, inc), glm::vec3(0.0, 0.0, 0.0));
	}
}

void cranePed(Crane* crane, bool dir) {
	Obj* targ = crane->_parent->_child[2 * 2 * 2 * 2]->_child[0];

	float inc = dir ? 1 : -1;

	glm::mat4 d = glm::translate(glm::mat4(1.0), glm::vec3(0.0, inc, 0.0));
	glm::vec3 loc = glm::vec3(targ->_acc * d * glm::vec4(glm::vec3(0.0), 1.0));

	if (
		loc[Y] > crane->_rngClaw[MIN] &&
		loc[Y] < crane->_rngClaw[MAX]
	) {
		glm::vec3 loc = glm::vec3(
			0.0,
			inc,
			0.0
		);

		std::thread t(objAnim, targ, loc, glm::vec3(0.0));
		t.detach();
	}
}

void craneGrab(Crane* crane) {
	std::this_thread::sleep_for(std::chrono::milliseconds(1000));

	Obj* a = scn[0]->_child[2 * 2 * 2 * 2]->_child[0];
	Obj* b = scn[1];

	glm::vec3 btm = glm::vec3(a->_acc * glm::vec4(glm::vec3(0.0, a->_rng[Y][MIN], 0.0), 1.0));
	glm::vec3 top = glm::vec3(b->_acc * glm::vec4(glm::vec3(0.0, b->_rng[Y][MAX], 0.0), 1.0));

	if (btm[Y] < top[Y]) {
		crane->_parent->_child[0] = b;
	}
}
