#include <GL/glew.h>

#include "lim.h"
#include "util.h"
#include "layout.h"

Lim* limMk(glm::vec3 loc, glm::vec3 rot) {
	Lim* _ = (Lim*) malloc(sizeof (Lim));

	std::vector<GLfloat> vtc = util::mesh::quad::pos(glm::vec2(
		layout::pad,
		100.0
	));

	std::vector<GLushort> idc = {
		0, 1, 2, 3
	};
	std::vector<GLushort> strip = util::mesh::strip(idc);

	_->_parent = objMk(&vtc[0], vtc.size() * sizeof (GLfloat), &strip[0], strip.size() * sizeof (GLushort), "obj", "solid", true, loc, rot);

	return _;
}
