#include <GL/glew.h>

#include "lim.h"
#include "util.h"
#include "layout.h"

Lim* limMk(unsigned int axis, GLfloat val, glm::vec3 loc, glm::vec3 rot) {
	Lim* _ = (Lim*) malloc(sizeof (Lim));

	_->_axis = axis;
	_->_val = val;

	std::vector<GLfloat> vtc = util::mesh::rect::pos(glm::vec2(
		layout::pad,
		100.0
	), Y, true);

	std::vector<GLushort> idc;
	for (int i = 0; i < 2 * 2; i++) {
		idc.push_back(i);
	}
	std::vector<GLushort> strip = util::mesh::strip(idc);

	_->_parent = objMk(&vtc[0], vtc.size() * sizeof (GLfloat), &strip[0], strip.size() * sizeof (GLushort), "obj", "solid", true, loc, rot);

	return _;
}
