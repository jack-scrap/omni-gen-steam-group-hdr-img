#include "layout.h"

unsigned int layout::view[2];

unsigned int layout::res[2];

unsigned int layout::canv[2];

glm::vec2 layout::item(glm::vec2 bound) {
	glm::vec2 _;

	for (int i = 0; i < 2; i++) {
		_[i] = bound[i] + (margin * 2);
	}

	return _;
}

glm::vec2 layout::bordered(glm::vec2 bound) {
	return item(bound) + item(glm::vec2(stroke * 2, stroke * 2));
}

glm::vec2 layout::var(glm::vec2 bound) {
	return bordered(item(glm::vec2(0.0, layout::letter[Y])) + bound);
}

glm::vec2 layout::center(glm::vec2 bound) {
	glm::vec2 _;

	for (int i = 0; i < 2; i++) {
		_[i] = bound[i] / 2;
	}

	return _;
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
