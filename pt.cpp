#include <cstdlib>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>

#include "pt.h"
#include "scn.h"

Pt* ptMk(glm::vec3 loc, glm::vec3 rot) {
	Pt* _ = (Pt*) malloc(sizeof (Pt));

	GLfloat vtc[3] = {
		0.0, 0.0, 0.0
	};

	GLushort idc[1] = {
		0
	};

	glm::mat4 locMatr = glm::translate(glm::mat4(1.0), loc);

	glm::mat4 rotMatr = glm::mat4(1.0);
	for (int i = 0; i < 3; i++) {
		glm::vec3 axis = glm::vec3(0);
		axis[i] = 1;

		rotMatr = glm::rotate(rotMatr, rot[i], axis);
	}
	_->_parent = objMk(vtc, sizeof vtc / sizeof *vtc, idc, sizeof idc / sizeof *idc, "tex", "bed", "tex", false, locMatr, rotMatr);

	return _;
}
