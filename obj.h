#pragma once

#include <cstdlib>
#include <GL/glew.h>

#include "disp.h"
#include "prog.h"

typedef struct {
	GLfloat _loc[3];

	GLuint
		_vao,
		_vbo,
		_ibo;

	GLint
		_attrPos,

		_uniLoc;

	Prog _prog;

	unsigned int _noIdc;
} Obj;

Obj objMk(GLfloat* vtc, unsigned int noVtc, unsigned short* idc, unsigned int noIdc, GLfloat* loc);

extern "C" void objDraw(Obj* obj);
