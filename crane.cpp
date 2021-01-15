#include <cstdlib>
#include <glm/gtc/matrix_transform.hpp>
#include <thread>

#include "crane.h"
#include "scn.h"
#include "math.h"
#include "state.h"

Crane* craneMk(glm::vec3 loc, glm::vec3 rot) {
	Crane* _ = (Crane*) malloc(sizeof (Crane));

	Obj* child[(2 * 2 * 2 * 2) + 1 + (2 * 2)];
	int i = 0;
	for (int z = 0; z < 2; z++) {
		for (int x = 0; x < 2; x++) {
			for (int j = 0; j < 2; j++) {
				for (int k = 0; k < 2; k++) {
					Obj* rim[] = {
						objMk("rim", "obj", "dir", true, loc + glm::vec3(((x ? 1 : -1) * 3.0) + (j ? 1 : -1), 1.0, ((z ? 1 : -1) * 10.0) + ((k ? 1 : -1) * 0.6)), rot)
					};

					child[i] = objMk("wheel", "obj", "dir", false, rim, 1, loc + glm::vec3(((x ? 1 : -1) * 3.0) + (j ? 1 : -1), 1.0, ((z ? 1 : -1) * 10.0) + ((k ? 1 : -1) * 0.6)), rot);

					i++;
				}
			}
		}
	}

	Obj* claw[] = {
		objMk("crane/claw", "obj", "dir", true, loc + glm::vec3(0.0, 10.8, 0.0), rot)
	};

	child[2 * 2 * 2 * 2] = objMk("crane/head", "obj", "dir", true, claw, 1, loc + glm::vec3(0.0, 13.8, 0.0), rot);

	std::vector<GLfloat> vtc = util::mesh::quad::pos(glm::vec2(0.6, 1.0));

	std::vector<GLushort> idc;
	for (int i = 0; i < 2 * 2; i++) {
		idc.push_back(i);
	}
	std::vector<GLushort> strip = util::mesh::strip(idc);

	i = 0;
	for (int z = 0; z < 2; z++) {
		for (int x = 0; x < 2; x++) {
			child[(2 * 2 * 2 * 2) + 1 + i] = objMk(&vtc[0], vtc.size(), &strip[0], strip.size(), "obj", "alert", false, loc + glm::vec3((x ? 1 : -1) * (6.0 + (state::pad * 2)), 1.74, (z ? 1 : -1) * 10.0), rot + glm::vec3(0.0, M_PI / 2, 0.0));

			i++;
		}
	}

	_->_parent = objMk("crane/body", "obj", "dir", true, child, sizeof child / sizeof *child, loc, rot);

	return _;
}

void craneAnim(Crane* crane, GLfloat* loc) {
	for (int i = 0; i < 2 * 2; i++) {
		crane->_parent->_child[(2 * 2 * 2 * 2) + 1 + i]->_active = true;
	}

	std::thread t(objAnim, crane->_parent, glm::vec3(loc[0], loc[1], loc[2]), glm::vec3(0.0));
	t.detach();

	std::this_thread::sleep_for(std::chrono::milliseconds(1000));

	for (int i = 0; i < 2 * 2; i++) {
		crane->_parent->_child[(2 * 2 * 2 * 2) + 1 + i]->_active = false;
	}
}

void craneMv(Crane* crane, GLfloat* loc) {
	std::thread t(craneAnim, crane, loc);
	t.detach();
}

void cranePan(Crane* crane, bool dir) {
	Obj* targ = crane->_parent->_child[2 * 2 * 2 * 2 * 2];

	glm::vec3 loc = targ->_model * glm::vec4(glm::vec3(0.0), 1.0);

	GLfloat d = dir ? 1 : -1;

	if (
		loc[Z] < 3.0 &&
		loc[Z] > -3.0
	) {
		GLfloat loc[3] = {
			d,
			d,
			d
		};
		objMv(crane->_parent->_child[2 * 2 * 2 * 2 * 2], loc);
	}
}

void cranePed(Crane* crane, bool dir) {
	/* Obj* targ = crane->_parent->_child[2 * 2 * 2 * 2 * 2]; */

	/* glm::vec3 loc = targ->_model * glm::vec4(glm::vec3(0.0), 1.0); */

	/* GLfloat d = dir ? 1 : -1; */

	/* if ( */
	/* 	loc[Y] < 13.8 && */
	/* 	loc[Y] > 0.0 */
	/* ) { */
	/* 	GLfloat loc[3] = { */
	/* 		d, */
	/* 		d, */
	/* 		d */
	/* 	}; */
	/* 	objMv(crane->_parent->_child[2 * 2 * 2 * 2 * 2], loc); */
	/* } */
}

void craneGrab(Crane* crane) {
	/* Obj* targ = crane->_parent->_child[2 * 2 * 2 * 2 * 2]; */

	/* targ->_noChild++; */
	/* targ->_child = (Obj**) realloc(targ->_child, targ->_noChild * sizeof (Obj*)); */

	/* targ->_child[targ->_noChild - 1] = obj[0]; */
}
