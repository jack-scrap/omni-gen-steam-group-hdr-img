#include "layout.h"

unsigned int layout::view[2];

unsigned int layout::res[2];

glm::vec2 layout::item(glm::vec2 bound) {
	glm::vec2 _;

	for (int i = 0; i < 2; i++) {
		_[i] = bound[i] + (margin * 2 * 2);
	}

	return _;
}

glm::vec2 layout::sz(glm::vec2 dim) {
	return glm::vec2(
		(dim[X] * stride[X]) + (pad * 2),
		(dim[Y] * stride[Z]) + (pad * 2)
	);
}

glm::vec3 layout::center(Obj* obj) {
	glm::vec3 sz;
	for (int a = 0; a < 3; a++) {
		sz[a] = obj->_aabb[a][MAX] - obj->_aabb[a][MIN];
	}

	glm::vec3 _;
	for (int a = 0; a < 3; a++) {
		sz[a] = obj->_aabb[a][MAX] - obj->_aabb[a][MIN];

		_[a] = -(sz[a] / 2);
	}

	return _;
}
