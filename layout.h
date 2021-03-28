#pragma once

#include <glm/glm.hpp>

#include "state.h"

namespace layout {
	static glm::vec2 dim = glm::vec2(16, 32);

	glm::vec2 view;

	glm::vec2 res = glm::vec2(state::ln * dim[X], view[Y]);

	static GLfloat bound = abs(-1.0 - 1.0);

	static constexpr GLfloat
		pad = 0.16,
		margin = 0.16,
		stroke = pad * 2,

		offset = stroke + margin,

		idx[2] = {
			2,
			4
		},

		glyph[2] = {
			0.8,
			1
		};
}
