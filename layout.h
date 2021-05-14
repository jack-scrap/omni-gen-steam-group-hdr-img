#pragma once

#include <glm/glm.hpp>

#include "state.h"

namespace layout {
	static glm::vec2 dim = glm::vec2(16, 32);

	extern glm::vec2 view;

	extern glm::vec2 res;

	static GLfloat bound = abs(-1.0 - 1.0);

	static constexpr GLfloat
		pad = 0.16,
		margin = 0.16,
		stroke = pad * 2,

		offset = stroke + margin,

		idx[3] = {
			2,
			2,
			4
		},

		stride[3] = {
			idx[X] + (margin * 2) + (margin * 2 * 2) + (stroke * 2),
			idx[Y],
			idx[Z] + (margin * 2) + (margin * 2 * 2) + (stroke * 2)
		},

		glyph[2] = {
			0.8,
			1
		};
}
