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
	glm::vec3 _loc;

	unsigned int _noIdc;

	bool _active;

	glm::mat4
		_model,
		_view,
		_proj;

	Prog _prog;

	GLuint _id[3];

	GLint
		_attr[1],

		_uni[5];

	struct Obj** _child;
	unsigned int _noChild;
} Obj;

Obj* objMk(GLfloat* vtc, unsigned int noVtc, GLushort* idc, unsigned int noIdc, std::string nameVtx, std::string nameFrag, bool active, glm::vec3 loc = glm::vec3(0.0, 0.0, 0.0));

Obj* objMk(GLfloat* vtc, unsigned int noVtc, GLushort* idc, unsigned int noIdc, std::string nameVtx, std::string nameFrag, bool active, Obj* child[], unsigned int noChild, glm::vec3 loc = glm::vec3(0.0, 0.0, 0.0));

Obj* objMk(GLfloat* vtc, unsigned int noVtc, GLushort* idc, unsigned int noIdc, std::string nameVtx, std::string nameGeom, std::string nameFrag, bool active, glm::vec3 loc = glm::vec3(0.0, 0.0, 0.0));

Obj* objMk(GLfloat* vtc, unsigned int noVtc, GLushort* idc, unsigned int noIdc, std::string nameVtx, std::string nameGeom, std::string nameFrag, bool active, Obj* child[], unsigned int noChild, glm::vec3 loc = glm::vec3(0.0, 0.0, 0.0));

Obj* objMk(std::string name, std::string nameVtx, std::string nameFrag, bool active, glm::vec3 loc = glm::vec3(0.0, 0.0, 0.0));

Obj* objMk(std::string name, std::string nameVtx, std::string nameFrag, bool active, Obj* child[], unsigned int noChild, glm::vec3 loc);

extern "C" void objMv(Obj* obj, GLfloat* d);

extern "C" void objDraw(Obj* obj);

extern "C" void set(Obj* obj, GLfloat* loc);
