#pragma once

#include <glm/glm.hpp>

#include "state.h"
#include "obj.h"

namespace layout {
	static unsigned int glyph[2] = {
		16,
		32
	};
	static unsigned int map[2] = {
		16,
		8
	};

	extern unsigned int view[2];

	extern unsigned int res[2];

	extern unsigned int canv[2];

	static GLfloat bound = abs(-1.0 - 1.0);

	static constexpr GLfloat idx[3] = {
		2,
		2,
		4
	};

	static constexpr GLfloat pad = 0.16;
	static constexpr GLfloat margin = 0.16;
	static constexpr GLfloat stroke = pad * 2;

	static constexpr GLfloat overhead = stroke * 2;
	static constexpr GLfloat offset = stroke + margin;

	static constexpr GLfloat stride[3] = {
		(margin * 2) + idx[X] + (margin * 2) + (stroke * 2) + (margin * 2),
		idx[Y],
		(margin * 2) + idx[Z] + (margin * 2) + (margin * 2) + (stroke * 2) + (margin * 2)
	};

	static constexpr GLfloat letter[2] = {
		0.8,
		1
	};

	glm::vec2 padded(glm::vec2 bound);
	glm::vec2 margined(glm::vec2 bound);
	glm::vec2 array(glm::vec2 bound);

	glm::vec3 center(Obj* obj);
}
