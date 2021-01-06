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

	GLfloat vtc[2 * 2 * 3];
	i = 0;
	for (int x = 0; x < 2; x++) {
		for (int y = 0; y < 2; y++) {
			vtc[i] = (x ? 1 : -1) * 0.6;
			vtc[i + 1] = y ? 1 : -1;
			vtc[i + 2] = 0.0;

			i += 3;
		}
	}

	GLushort idc[] = {
		0, 1, 2,
		2, 1, 3
	};

	i = 0;
	for (int z = 0; z < 2; z++) {
		for (int x = 0; x < 2; x++) {
			child[(2 * 2 * 2 * 2) + 1 + i] = objMk(vtc, sizeof vtc / sizeof *vtc, idc, sizeof idc / sizeof *idc, "main", "alert", true, loc + glm::vec3((x ? 1 : -1) * (6.0 + (state::pad * 2)), 1.74, (z ? 1 : -1) * 10.0), glm::vec3(0.0, M_PI / 2, 0.0));

			i++;
		}
	}

	_->_parent = objMk("crane/body", "main", "dir", true, child, sizeof child / sizeof *child, loc);

	return _;
}

void craneMv(Crane* crane, GLfloat* d) {
	objMv(crane->_parent, d);
}

void cranePan(Crane* crane, bool dir) {
	if (dir) {
		if (crane->_parent->_child[2 * 2 * 2 * 2 * 2]->_loc[Z] < 3.0) {
			crane->_parent->_child[2 * 2 * 2 * 2 * 2]->_loc[Z]++;
		}
	} else {
		if (crane->_parent->_child[2 * 2 * 2 * 2 * 2]->_loc[Z] > -3.0) {
			crane->_parent->_child[2 * 2 * 2 * 2 * 2]->_loc[Z]--;
		}
	}

	GLfloat loc[3];
	for (int i = 0; i < 3; i++) {
		loc[i] = crane->_parent->_loc[i];
	}
	objMv(crane->_parent->_child[2 * 2 * 2 * 2 * 2], loc);
}

void cranePed(Crane* crane, bool dir) {
	if (dir) {
		if (crane->_parent->_child[2 * 2 * 2 * 2 * 2]->_child[0]->_loc[Y] < 13.8) {
			crane->_parent->_child[2 * 2 * 2 * 2 * 2]->_child[0]->_loc[Y]++;
		}
	} else {
		if (crane->_parent->_child[2 * 2 * 2 * 2 * 2]->_child[0]->_loc[Y] > 0.0) {
			crane->_parent->_child[2 * 2 * 2 * 2 * 2]->_child[0]->_loc[Y]--;
		}
	}

	GLfloat loc[3];
	for (int i = 0; i < 3; i++) {
		loc[i] = crane->_parent->_loc[i];
	}
	objMv(crane->_parent->_child[2 * 2 * 2 * 2 * 2], loc);
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
