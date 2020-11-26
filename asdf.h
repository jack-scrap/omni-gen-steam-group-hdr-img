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
} Asdf;

Asdf asdfMk(GLfloat* loc);

extern "C" void asdfDraw(Asdf* asdf);

extern "C" void set(Asdf* asdf, GLfloat* loc);
