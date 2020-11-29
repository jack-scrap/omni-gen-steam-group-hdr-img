#pragma once

#include <cstdlib>
#include <GL/glew.h>

#include "disp.h"
#include "prog.h"

typedef struct {
	GLfloat _loc[2];

	GLuint
		_vao,
		_vbo;

	GLint
		_attrPos,

		_uniLoc;

	Prog _prog;
} Poly;

Poly polyMk(GLfloat* loc);

extern "C" void polyDraw(Poly* poly);

/* extern "C" void set(Poly* poly, GLfloat* loc); */
