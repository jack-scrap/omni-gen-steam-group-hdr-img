#pragma once

#include <cstdlib>
#include <GL/glew.h>

#include "disp.h"
#include "prog.h"

enum id {
	VAO,
	VBO,
	IBO
};

enum attr {
	POS
};

enum uni {
	LOC
};

typedef struct {
	GLfloat _loc[3];

	GLuint _id[3];

	GLint
		_attr[1],

		_uni[1];

	Prog _prog;

	unsigned int _noIdc;
} Obj;

Obj objMk(GLfloat* vtc, unsigned int noVtc, unsigned short* idc, unsigned int noIdc, GLfloat* loc);

extern "C" void objDraw(Obj* obj);
