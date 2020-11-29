#pragma once

#include <cstdlib>
#include <GL/glew.h>
#include <glm/glm.hpp>
#include <string>

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
	glm::vec3 _loc;

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

Obj objMk(GLfloat* vtc, unsigned int noVtc, GLushort* idc, unsigned int noIdc, glm::vec3 loc = glm::vec3(0.0, 0.0, 0.0));

Obj objMk(std::string name, glm::vec3 loc = glm::vec3(0.0, 0.0, 0.0));

extern "C" void objDraw(Obj* obj);

extern "C" void set(Obj* obj, GLfloat* loc);
