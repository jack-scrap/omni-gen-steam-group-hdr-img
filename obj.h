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
	STBO,
	IBO
};

enum attr {
	POS,
	ST
};

enum uni {
	MODEL,
	VIEW,
	PROJ,

	LOC,

	ACTIVE,

	T
};

typedef struct Obj {
	GLuint _id[4];

	unsigned int _noIdc;

	GLfloat _bound[2][3];

	glm::vec3
		_loc,
		_rot;

	GLfloat _v;

	bool _active;

	unsigned int _t;

	glm::mat4
		_model,
		_view,
		_proj;

	Prog _prog;

	GLint
		_attr[3],
		_uni[6];

	struct Obj** _child;
	unsigned int _noChild;
} Obj;

Obj* objMk(GLfloat* vtc, unsigned int noVtc, GLushort* idc, unsigned int noIdc, std::string nameVtx, std::string nameFrag, bool active, glm::vec3 loc = glm::vec3(0.0, 0.0, 0.0), glm::vec3 rot = glm::vec3(0.0, 0.0, 0.0));

Obj* objMk(GLfloat* vtc, unsigned int noVtc, GLushort* idc, unsigned int noIdc, std::string nameVtx, std::string nameFrag, bool active, Obj* child[], unsigned int noChild, glm::vec3 loc = glm::vec3(0.0, 0.0, 0.0), glm::vec3 rot = glm::vec3(0.0, 0.0, 0.0));

Obj* objMk(GLfloat* vtc, unsigned int noVtc, GLushort* idc, unsigned int noIdc, std::string nameVtx, std::string nameGeom, std::string nameFrag, bool active, glm::vec3 loc = glm::vec3(0.0, 0.0, 0.0), glm::vec3 rot = glm::vec3(0.0, 0.0, 0.0));

Obj* objMk(GLfloat* vtc, unsigned int noVtc, GLushort* idc, unsigned int noIdc, std::string nameVtx, std::string nameGeom, std::string nameFrag, bool active, Obj* child[], unsigned int noChild, glm::vec3 loc = glm::vec3(0.0, 0.0, 0.0), glm::vec3 rot = glm::vec3(0.0, 0.0, 0.0));

Obj* objMk(std::string name, std::string nameVtx, std::string nameFrag, bool active, glm::vec3 loc = glm::vec3(0.0, 0.0, 0.0), glm::vec3 rot = glm::vec3(0.0, 0.0, 0.0));

Obj* objMk(std::string name, std::string nameVtx, std::string nameFrag, bool active, Obj* child[], unsigned int noChild, glm::vec3 loc, glm::vec3 rot = glm::vec3(0.0, 0.0, 0.0));

extern "C" void objMv(Obj* obj, GLfloat* d);

extern "C" void objSet(Obj* obj, GLfloat* d);

extern "C" void objA(Obj* obj);

extern "C" void objDraw(Obj* obj);
