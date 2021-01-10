#include <cstdlib>
#include <glm/gtc/matrix_transform.hpp>
#include <thread>

#include "crane.h"
#include "scn.h"
#include "math.h"
#include "state.h"

Crane* craneMk(glm::vec3 loc) {
	Crane* _ = (Crane*) malloc(sizeof (Crane));

	Obj* child[(2 * 2 * 2 * 2) + 1 + (2 * 2)];
	int i = 0;
	for (int z = 0; z < 2; z++) {
		for (int x = 0; x < 2; x++) {
			for (int j = 0; j < 2; j++) {
				for (int k = 0; k < 2; k++) {
					Obj* rim[] = {
						objMk("rim", "main", "dir", true, loc + glm::vec3(((x ? 1 : -1) * 3.0) + (j ? 1 : -1), 1.0, ((z ? 1 : -1) * 10.0) + ((k ? 1 : -1) * 0.6)))
					};

					child[i] = objMk("wheel", "main", "dir", false, rim, 1, loc + glm::vec3(((x ? 1 : -1) * 3.0) + (j ? 1 : -1), 1.0, ((z ? 1 : -1) * 10.0) + ((k ? 1 : -1) * 0.6)));

					i++;
				}
			}
		}
	}

	Obj* claw[] = {
		objMk("crane/claw", "main", "dir", true, loc + glm::vec3(0.0, 10.8, 0.0))
	};

	child[2 * 2 * 2 * 2] = objMk("crane/head", "main", "dir", true, claw, 1, loc + glm::vec3(0.0, 13.8, 0.0));

	std::vector<GLfloat> vtc = util::mesh::quad::pos(glm::vec2(0.6, 1.0));

	std::vector<GLushort> idc;
	for (int i = 0; i < 2 * 2; i++) {
		idc.push_back(i);
	}
	std::vector<GLushort> strip = util::mesh::strip(idc);

	i = 0;
	for (int z = 0; z < 2; z++) {
		for (int x = 0; x < 2; x++) {
			child[(2 * 2 * 2 * 2) + 1 + i] = objMk(&vtc[0], vtc.size(), &strip[0], strip.size(), "main", "alert", false, loc + glm::vec3((x ? 1 : -1) * (6.0 + (state::pad * 2)), 1.74, (z ? 1 : -1) * 10.0), glm::vec3(0.0, M_PI / 2, 0.0));

			i++;
		}
	}

	_->_parent = objMk("crane/body", "main", "dir", true, child, sizeof child / sizeof *child, loc);

	return _;
}

void craneUpdate(Obj* obj, glm::vec3 loc, glm::vec3 rot) {
	obj->_model = glm::translate(obj->_model, loc);
	for (int i = 0; i < 3; i++) {
		glm::vec3 axis = glm::vec3(0);
		axis[i] = 1;

		obj->_model = glm::rotate(obj->_model, rot[i], axis);
	}

	for (int i = 0; i < obj->_noChild; i++) {
		if (obj->_child[i]) {
			craneUpdate(obj->_child[i], loc, rot);
		}
	}
}

void craneAnim(Crane* crane, glm::vec3 loc, glm::vec3 rot) {
	int i = 0;
	for (int y = 0; y < 2; y++) {
		for (int x = 0; x < 2; x++) {
			crane->_parent->_child[(2 * 2 * 2 * 2) + 1 + i]->_active = true;

			i++;
		}
	}

	int t = 0;
	while (t < 10) {
		craneUpdate(crane->_parent, loc / glm::vec3(state::fps), rot / glm::vec3(state::fps));

		std::this_thread::sleep_for(std::chrono::milliseconds(1000 / state::fps));

		t++;
	}

	i = 0;
	for (int y = 0; y < 2; y++) {
		for (int x = 0; x < 2; x++) {
			crane->_parent->_child[(2 * 2 * 2 * 2) + 1 + i]->_active = false;

			i++;
		}
	}
}

void craneMv(Crane* crane, GLfloat* loc) {
	std::thread t(craneAnim, crane, glm::vec3(loc[0], loc[1], loc[2]), glm::vec3(0.0, 0.0, 0.0));
	t.detach();
}

void cranePan(Crane* crane, bool dir) {
	/* if ( */
	/* 	crane->_parent->_child[2 * 2 * 2 * 2 * 2]->_loc[Z] < 3.0 && */
	/* 	crane->_parent->_child[2 * 2 * 2 * 2 * 2]->_loc[Z] > -3.0 */
	/* ) { */
	/* 	crane->_parent->_child[2 * 2 * 2 * 2 * 2]->_loc[Z] += dir ? 1 : -1; */
	/* } */

	/* GLfloat loc[3]; */
	/* for (int i = 0; i < 3; i++) { */
	/* 	loc[i] = crane->_parent->_loc[i]; */
	/* } */
	/* objMv(crane->_parent->_child[2 * 2 * 2 * 2 * 2], loc); */
}

void cranePed(Crane* crane, bool dir) {
	/* if ( */
	/* 	crane->_parent->_child[2 * 2 * 2 * 2 * 2]->_child[0]->_loc[Y] < 13.8 && */
	/* 	crane->_parent->_child[2 * 2 * 2 * 2 * 2]->_child[0]->_loc[Y] > 0.0 */
	/* ) { */
	/* 	crane->_parent->_child[2 * 2 * 2 * 2 * 2]->_child[0]->_loc[Y] += dir ? 1 : -1; */
	/* } */

	/* GLfloat loc[3]; */
	/* for (int i = 0; i < 3; i++) { */
	/* 	loc[i] = crane->_parent->_loc[i]; */
	/* } */
	/* objMv(crane->_parent->_child[2 * 2 * 2 * 2 * 2], loc); */
}

void craneGrab(Crane* crane) {
	Obj* targ = crane->_parent->_child[2 * 2 * 2 * 2 * 2];

	targ->_noChild++;
	targ->_child = (Obj**) realloc(targ->_child, targ->_noChild * sizeof (Obj*));

	targ->_child[targ->_noChild - 1] = obj[0];
}

void craneDraw(Crane* crane) {
	objDraw(crane->_parent);
}
