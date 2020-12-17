#pragma once

#include <GL/glew.h>
#include <glm/glm.hpp>

#include "math.h"

namespace state {
	static unsigned int
		line = 18,
		ln = 36;

	static glm::vec2 dim = glm::vec2(16, 32);

	static glm::vec2 view = glm::vec2(800, line * dim[Y]);

	static glm::vec2 res = glm::vec2(ln * dim[X], view[Y]);

	static GLfloat bound = abs(-1.0 - 1.0);

	static glm::vec2
		pc = bound / res,

		step = pc * dim;
}
