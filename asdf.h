#pragma once

#include <cstdlib>
#include <GL/glew.h>

#include "disp.h"
#include "prog.h"

typedef struct {
	GLfloat
		_x,
		_y;

	GLuint
		_vao,
		_vbo;

	GLint
		_attrPos,

		_uniLoc;

	Prog _prog;
} Asdf;

Asdf asdfMk();

extern "C" void asdfDraw(Asdf* asdf);

extern "C" void set(Asdf* asdf, GLfloat x, GLfloat y);
