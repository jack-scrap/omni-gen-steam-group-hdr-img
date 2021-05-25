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
