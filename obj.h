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

	LOC,

	ACTIVE
};

typedef struct Obj {
	unsigned int _noIdc;

	glm::vec3 _loc;

	glm::mat4
		_model,
		_view,
		_proj;

	Prog _prog;

	GLuint _id[3];

	GLint
		_attr[1],

		_uni[5];

	struct Obj* _child;
} Obj;

Obj objMk(GLfloat* vtc, unsigned int noVtc, GLushort* idc, unsigned int noIdc, bool active, glm::vec3 loc = glm::vec3(0.0, 0.0, 0.0));

Obj objMk(std::string name, bool active, glm::vec3 loc = glm::vec3(0.0, 0.0, 0.0));

extern "C" void objDraw(Obj* obj);

extern "C" void set(Obj* obj, GLfloat* loc);
