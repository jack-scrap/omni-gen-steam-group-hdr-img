#pragma once

#include <cstdlib>
#include <GL/glew.h>
#include <glm/glm.hpp>

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
	MODEL,
	VIEW,
	PROJ,

	LOC
};

typedef struct {
	GLfloat _loc[3];

	GLuint _id[3];

	GLint
		_attr[1],

		_uni[4];

	Prog _prog;

	glm::mat4
		_proj,
		_view,
		_model;

	unsigned int _noIdc;
} Obj;

Obj objMk(GLfloat* vtc, unsigned int noVtc, unsigned short* idc, unsigned int noIdc, GLfloat* loc);

extern "C" void objDraw(Obj* obj);
