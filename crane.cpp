#include <cstdlib>
#include <glm/gtc/matrix_transform.hpp>
#include <thread>

#include "crane.h"
#include "scn.h"
#include "math.h"

Crane* craneMk(glm::vec3 loc) {
	Crane* _ = (Crane*) malloc(sizeof (Crane));

	Obj* child[(2 * 2 * 2 * 2 * 2) + 1];
	int i = 0;
	for (int z = 0; z < 2; z++) {
		for (int x = 0; x < 2; x++) {
			for (int j = 0; j < 2; j++) {
				for (int k = 0; k < 2; k++) {
					child[i] = objMk("wheel", "main", "dir", false, loc + glm::vec3(((x ? 1 : -1) * 3.0) + (j ? 1 : -1), 1.0, ((z ? 1 : -1) * 10.0) + ((k ? 1 : -1) * 0.6)));

					i++;

					child[i] = objMk("rim", "main", "dir", true, loc + glm::vec3(((x ? 1 : -1) * 3.0) + (j ? 1 : -1), 1.0, ((z ? 1 : -1) * 10.0) + ((k ? 1 : -1) * 0.6)));

					i++;
				}
			}
		}
	}

	Obj* claw[] = {
		objMk("crane/claw", "main", "dir", true, loc + glm::vec3(0.0, 10.8, 0.0))
	};

	child[2 * 2 * 2 * 2 * 2] = objMk("crane/head", "main", "dir", true, claw, 1, loc + glm::vec3(0.0, 13.8, 0.0));

	_->_parent = objMk("crane/body", "main", "dir", true, child, sizeof child / sizeof *child, loc);

	return _;
}

void cranePan(Crane* crane, bool dir) {
	if (dir) {
		if (crane->_parent->_child[2 * 2 * 2]->_loc[Z] < 3) {
			crane->_parent->_child[2 * 2 * 2]->_loc[Z]++;
		}
	} else {
		if (crane->_parent->_child[2 * 2 * 2]->_loc[Z] > -3) {
			crane->_parent->_child[2 * 2 * 2]->_loc[Z]--;
		}
	}

	GLfloat loc[3];
	for (int i = 0; i < 3; i++) {
		loc[i] = crane->_parent->_loc[i];
	}
	objMv(crane->_parent->_child[2 * 2 * 2], loc);
}

void cranePed(Crane* crane, bool dir) {
	if (dir) {
		if (crane->_parent->_child[2 * 2 * 2]->_loc[Y] < 13.8) {
			crane->_parent->_child[2 * 2 * 2]->_loc[Y]++;
		}
	} else {
		if (crane->_parent->_child[2 * 2 * 2]->_loc[Y] > 0) {
			crane->_parent->_child[2 * 2 * 2]->_loc[Y]--;
		}
	}

	GLfloat loc[3];
	for (int i = 0; i < 3; i++) {
		loc[i] = crane->_parent->_loc[i];
	}
	objMv(crane->_parent->_child[2 * 2 * 2], loc);
}

Crane* craneGet() {
	return crane;
}

void craneSet(Crane* crane, GLfloat* d) {
	unsigned int fps = 10;

	float step[3];
	for (int i = 0; i < 3; i++) {
		step[i] = d[i] / fps;
	}

	for (int t = 0; t < fps; t++) {
		for (int i = 0; i < 3; i++) {
			crane->_parent->_loc[i] += step[i];
		}

		crane->_parent->_model = glm::translate(glm::mat4(1.0), crane->_parent->_loc);

		std::this_thread::sleep_for(std::chrono::milliseconds(1000 / fps));
	}
}

void craneDraw(Crane* crane) {
	objDraw(crane->_parent);
}
