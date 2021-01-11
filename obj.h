#pragma once

#include <cstdlib>
#include <GL/glew.h>
#include <glm/glm.hpp>
#include <string>
#include <vector>

#include "disp.h"
#include "prog.h"
#include "mesh1.h"

enum attr {
	POS,
	ST
};

enum uni {
	MODEL,
	VIEW,
	PROJ,

	ACTIVE,

	T
};

typedef struct Obj {
	Mesh1* _mesh1;

	GLfloat _bound[2][3];

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
		_uni[5];

	struct Obj** _child;
	unsigned int _noChild;
} Obj;

Obj* objMk(GLfloat* vtc, unsigned int noVtc, GLushort* idc, unsigned int noIdc, std::string vtx, std::string frag, bool active, glm::vec3 loc = glm::vec3(0.0, 0.0, 0.0), glm::vec3 rot = glm::vec3(0.0, 0.0, 0.0));

Obj* objMk(GLfloat* vtc, unsigned int noVtc, GLushort* idc, unsigned int noIdc, std::string vtx, std::string frag, bool active, Obj** child, unsigned int noChild, glm::vec3 loc = glm::vec3(0.0, 0.0, 0.0), glm::vec3 rot = glm::vec3(0.0, 0.0, 0.0));

Obj* objMk(GLfloat* vtc, unsigned int noVtc, GLushort* idc, unsigned int noIdc, std::string vtx, std::string geom, std::string frag, bool active, glm::vec3 loc = glm::vec3(0.0, 0.0, 0.0), glm::vec3 rot = glm::vec3(0.0, 0.0, 0.0));

Obj* objMk(GLfloat* vtc, unsigned int noVtc, GLushort* idc, unsigned int noIdc, std::string vtx, std::string geom, std::string frag, bool active, Obj** child, unsigned int noChild, glm::vec3 loc = glm::vec3(0.0, 0.0, 0.0), glm::vec3 rot = glm::vec3(0.0, 0.0, 0.0));

Obj* objMk(std::string name, std::string vtx, std::string frag, bool active, glm::vec3 loc = glm::vec3(0.0, 0.0, 0.0), glm::vec3 rot = glm::vec3(0.0, 0.0, 0.0));

Obj* objMk(std::string name, std::string vtx, std::string frag, bool active, Obj** child, unsigned int noChild, glm::vec3 loc, glm::vec3 rot = glm::vec3(0.0, 0.0, 0.0));

void objUpdate(Obj* obj, glm::vec3 loc, glm::vec3 rot);

void objAnim(Obj* obj, glm::vec3 loc, glm::vec3 rot);

extern "C" void objMv(Obj* obj, GLfloat* loc);

void objA(Obj* obj);

void objDraw(Obj* obj);
