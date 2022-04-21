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

	bool _active;

	GLfloat _v;

	glm::mat4 _model;
	glm::mat4 _view;
	glm::mat4 _proj;

	glm::mat4 _acc;

	Prog _prog;

	GLint _attr[3];

	GLint* _uni;
	unsigned int _noUni = 5;

	GLuint _tex;

	GLfloat _aabb[3][2];

	Obj** _child;
	unsigned int _noChild;

	enum {
		POS,
		ST,
		NORM
	};

	enum {
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

Obj* objMk(std::string name, std::string vtx, std::string frag, std::string tex, bool active, glm::vec3 loc = glm::vec3(0.0), glm::vec3 rot = glm::vec3(0.0));

Obj* objMk(std::string name, std::string vtx, std::string frag, std::string tex, bool active, Obj** child, unsigned int noChild, glm::vec3 loc = glm::vec3(0.0), glm::vec3 rot = glm::vec3(0.0));

Obj* objMk(GLfloat* vtc, GLfloat* st, GLushort* idc, unsigned int noPrim, std::string vtx, std::string frag, std::string tex, bool active, glm::vec3 loc = glm::vec3(0.0), glm::vec3 rot = glm::vec3(0.0));

Obj* objMk(GLfloat* vtc, GLfloat* st, GLushort* idc, unsigned int noPrim, std::string vtx, std::string frag, std::string tex, bool active, Obj** child, unsigned int noChild, glm::vec3 loc = glm::vec3(0.0), glm::vec3 rot = glm::vec3(0.0));

Obj* objMk(GLfloat* vtc, GLushort* idc, unsigned int noPrim, std::string vtx, std::string geom, std::string frag, bool active, glm::vec3 loc = glm::vec3(0.0), glm::vec3 rot = glm::vec3(0.0));

Obj* objMk(GLfloat* vtc, GLushort* idc, unsigned int noPrim, std::string vtx, std::string geom, std::string frag, bool active, Obj** child, unsigned int noChild, glm::vec3 loc = glm::vec3(0.0), glm::vec3 rot = glm::vec3(0.0));

void objDel(Obj* obj);

void objAcc(Obj* obj, glm::mat4 prev);

void objAnim(Obj* obj, Obj* parent, glm::vec3 loc, glm::vec3 rot, GLfloat speed = 1.0);

void objMv(Obj* obj, Obj* parent, glm::vec3 loc, glm::vec3 rot, GLfloat speed = 1.0);

void objA(Obj* obj);

void objDraw(Obj* obj, unsigned int type);
