#include "layout.h"

unsigned int layout::view[2];

unsigned int layout::res[2];

unsigned int layout::canv[2];

glm::vec2 layout::padded(glm::vec2 bound) {
	glm::vec2 _;

	for (int i = 0; i < 2; i++) {
		_[i] = bound[i] + (pad * 2);
	}

	return _;
}

glm::vec2 layout::margined(glm::vec2 bound) {
	glm::vec2 _;

	for (int i = 0; i < 2; i++) {
		_[i] = bound[i] + (margin * 2);
	}

	return _;
}

glm::vec2 layout::array(glm::vec2 bound) {
	return glm::vec2(
		(bound[X] * stride[X]) + (pad * 2),
		(bound[Y] * stride[Z]) + (pad * 2)
	);
}

glm::vec3 layout::center(Obj* obj) {
	glm::vec3 bound;
	for (int a = 0; a < 3; a++) {
		bound[a] = obj->_aabb[a][MAX] - obj->_aabb[a][MIN];
	}

	glm::vec3 _;
	for (int a = 0; a < 3; a++) {
		bound[a] = obj->_aabb[a][MAX] - obj->_aabb[a][MIN];

		_[a] = -(bound[a] / 2);
	}

	return _;
}
