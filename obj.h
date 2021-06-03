#pragma once

#include <cstdlib>
#include <GL/glew.h>
#include <glm/glm.hpp>
#include <string>
#include <vector>

#include "disp.h"
#include "prog.h"
#include "mesh.h"

typedef struct Obj {
	Mesh* _mesh;

	GLfloat _v;

	bool _active;

	glm::mat4
		_model,
		_view,
		_proj,

		_acc;

	Prog _prog;

	GLint
		_attr[3],
		* _uni;

	GLfloat _bound[3][2];

	struct Obj** _child;
	unsigned int _noChild;

	GLuint _tex;

	enum attr {
		POS,
		ST,
		NORM
	};

	enum uni {
		MODEL,
		VIEW,
		PROJ,

		ACTIVE,

		T
	};
} Obj;

Obj* objMk(GLfloat* vtc, GLushort* idc, unsigned int noPrim, std::string vtx, std::string frag, bool active, glm::vec3 loc = glm::vec3(0.0), glm::vec3 rot = glm::vec3(0.0));

Obj* objMk(GLfloat* vtc, GLushort* idc, unsigned int noPrim, std::string vtx, std::string frag, bool active, Obj** child, unsigned int noChild, glm::vec3 loc = glm::vec3(0.0), glm::vec3 rot = glm::vec3(0.0));

Obj* objMk(std::string name, std::string vtx, std::string frag, bool active, glm::vec3 loc = glm::vec3(0.0), glm::vec3 rot = glm::vec3(0.0));

Obj* objMk(std::string name, std::string vtx, std::string frag, bool active, Obj** child, unsigned int noChild, glm::vec3 loc = glm::vec3(0.0), glm::vec3 rot = glm::vec3(0.0));

Obj* objMk(GLfloat* vtc, GLfloat* st, GLushort* idc, unsigned int noPrim, std::string vtx, std::string frag, bool active, glm::vec3 loc = glm::vec3(0.0), glm::vec3 rot = glm::vec3(0.0));

Obj* objMk(GLfloat* vtc, GLfloat* st, GLushort* idc, unsigned int noPrim, std::string vtx, std::string frag, bool active, Obj** child, unsigned int noChild, glm::vec3 loc = glm::vec3(0.0), glm::vec3 rot = glm::vec3(0.0));

Obj* objMk(GLfloat* vtc, GLushort* idc, unsigned int noPrim, std::string vtx, std::string geom, std::string frag, bool active, glm::vec3 loc = glm::vec3(0.0), glm::vec3 rot = glm::vec3(0.0));

Obj* objMk(GLfloat* vtc, GLushort* idc, unsigned int noPrim, std::string vtx, std::string geom, std::string frag, bool active, Obj** child, unsigned int noChild, glm::vec3 loc = glm::vec3(0.0), glm::vec3 rot = glm::vec3(0.0));

void objAcc(Obj* obj, glm::mat4 prev);

void objAnim(Obj* obj, glm::vec3 loc, glm::vec3 rot);

void objTrans(Obj* obj, glm::vec3 loc, glm::vec3 rot);

void objA(Obj* obj);

void objDraw(Obj* obj);
