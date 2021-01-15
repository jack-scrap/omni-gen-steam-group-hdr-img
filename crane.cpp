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
					glm::mat4
						locMatr = glm::translate(glm::mat4(1.0), loc + glm::vec3(((x ? 1 : -1) * 3.0) + (j ? 1 : -1), 1.0, ((z ? 1 : -1) * 10.0) + ((k ? 1 : -1) * 0.6))),
						rotMatr = glm::mat4(1.0);
					for (int i = 0; i < 3; i++) {
						glm::vec3 axis = glm::vec3(0);
						axis[i] = 1;

						rotMatr = glm::rotate(rotMatr, rot[i], axis);
					}

					Obj* rim[] = {
						objMk("rim", "obj", "dir", true, locMatr, rotMatr)
					};

					child[i] = objMk("wheel", "obj", "dir", false, rim, 1, locMatr, rotMatr);

					i++;
				}
			}
		}
	}

	glm::mat4
		locMatr = glm::translate(glm::mat4(1.0), glm::vec3(0.0, 10.8, 0.0)),
		rotMatr = glm::mat4(1.0);
	for (int i = 0; i < 3; i++) {
		glm::vec3 axis = glm::vec3(0);
		axis[i] = 1;

		rotMatr = glm::rotate(rotMatr, rot[i], axis);
	}

	Obj* claw[] = {
		objMk("crane/claw", "obj", "dir", true, locMatr, rotMatr)
	};

	glm::mat4
		locMatr1 = glm::translate(glm::mat4(1.0), loc + glm::vec3(0.0, 13.8, 0.0)),
		rotMatr1 = glm::mat4(1.0);
	for (int i = 0; i < 3; i++) {
		glm::vec3 axis = glm::vec3(0);
		axis[i] = 1;

		rotMatr1 = glm::rotate(rotMatr1, rot[i], axis);
	}

	child[2 * 2 * 2 * 2] = objMk("crane/head", "obj", "dir", true, claw, 1, locMatr1, rotMatr1);

	std::vector<GLfloat> vtc = util::mesh::quad::pos(glm::vec2(0.6, 1.0));

	std::vector<GLushort> idc;
	for (int i = 0; i < 2 * 2; i++) {
		idc.push_back(i);
	}
	std::vector<GLushort> strip = util::mesh::strip(idc);

	i = 0;
	for (int z = 0; z < 2; z++) {
		for (int x = 0; x < 2; x++) {
			glm::mat4 locMatr2 = glm::translate(glm::mat4(1.0), loc + glm::vec3(0.0, 13.8, 0.0));
			glm::mat4 rotMatr2 = glm::rotate(glm::mat4(1.0), (GLfloat) 0.0, rot);
			for (int i = 0; i < 3; i++) {
				glm::vec3 axis = glm::vec3(0);
				axis[i] = 1;

				rotMatr2 = glm::rotate(rotMatr1, rot[i], axis);
			}

			child[(2 * 2 * 2 * 2) + 1 + i] = objMk(&vtc[0], vtc.size(), &strip[0], strip.size(), "obj", "alert", false, locMatr2, rotMatr2);

			i++;
		}
	}

	glm::mat4
		locMatr3 = glm::translate(glm::mat4(1.0), loc),
		rotMatr3 = glm::mat4(1.0);
	for (int i = 0; i < 3; i++) {
		glm::vec3 axis = glm::vec3(0);
		axis[i] = 1;

		rotMatr3 = glm::rotate(rotMatr3, rot[i], axis);
	}

	_->_parent = objMk("crane/body", "obj", "dir", true, child, sizeof child / sizeof *child, locMatr3, rotMatr3);

	return _;
}

void craneAnim(Crane* crane, GLfloat* loc) {
	/* for (int i = 0; i < 2 * 2; i++) { */
	/* 	crane->_parent->_child[(2 * 2 * 2 * 2) + 1 + i]->_active = true; */
	/* } */

	/* std::thread t(objAnim, crane->_parent, glm::vec3(loc[0], loc[1], loc[2]), glm::vec3(0.0)); */
	/* t.detach(); */

	/* std::this_thread::sleep_for(std::chrono::milliseconds(1000)); */

	/* for (int i = 0; i < 2 * 2; i++) { */
	/* 	crane->_parent->_child[(2 * 2 * 2 * 2) + 1 + i]->_active = false; */
	/* } */
}

void craneMv(Crane* crane, GLfloat* loc) {
	/* std::thread t(craneAnim, crane, loc); */
	/* t.detach(); */
}

void cranePan(Crane* crane, bool dir) {
	/* Obj* targ = crane->_parent->_child[2 * 2 * 2 * 2 * 2]; */

	/* glm::vec3 loc = targ->_model * glm::vec4(glm::vec3(0.0), 1.0); */

	/* GLfloat d = dir ? 1 : -1; */

	/* if ( */
	/* 	loc[Z] < 3.0 && */
	/* 	loc[Z] > -3.0 */
	/* ) { */
	/* 	GLfloat loc[3] = { */
	/* 		d, */
	/* 		d, */
	/* 		d */
	/* 	}; */
	/* 	objMv(crane->_parent->_child[2 * 2 * 2 * 2 * 2], loc); */
	/* } */
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
