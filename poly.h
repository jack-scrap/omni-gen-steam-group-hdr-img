#pragma once

#include <cstdlib>
#include <GL/glew.h>

#include "disp.h"
#include "prog.h"

typedef struct {
	glm::vec3 _loc;

	GLuint
		_vao,
		_vbo;

	GLint
		_attrPos,

		_uniLoc;

	Prog _prog;
} Poly;

Poly polyMk(glm::vec3 loc);

extern "C" void polyDraw(Poly* poly);
