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

	_->_parent = objMk(vtc, sizeof vtc / sizeof *vtc, idc, sizeof idc / sizeof *idc, "tex", "bed", "tex", false, loc, rot);

	return _;
}
