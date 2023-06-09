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

	static constexpr GLfloat bound = abs(-1.0 - 1.0);

	static constexpr GLfloat idx[3] = {
		2,
		2,
		4
	};

	static constexpr GLfloat pad = 0.16;
	static constexpr GLfloat margin = 0.16;
	static constexpr GLfloat stroke = pad * 2;

	static constexpr GLfloat overhead = stroke + margin + margin;
	static constexpr GLfloat offset = stroke + margin;

	static constexpr GLfloat letter[2] = {
		0.8,
		1
	};

	GLfloat padded(GLfloat bound);
	GLfloat item(GLfloat bound);

	GLfloat scoped(GLfloat bound);

	glm::vec2 var(glm::vec2 bound);

	glm::vec2 center(glm::vec2 bound);
	glm::vec3 center(Obj* obj);
}
