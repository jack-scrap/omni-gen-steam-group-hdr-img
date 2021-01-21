#pragma once

#include <glm/glm.hpp>
#include <string>

#include "mesh.h"
#include "prog.h"

typedef struct {
	Mesh* _mesh;

	bool _active;

	unsigned int _t;

	glm::mat4
		_model,
		_loc,
		_rot,
		_view,
		_proj;

	Prog _prog;

	GLint
		_attr[3],
		_uni[5];
} Vec;

Vec* vecMk(GLfloat* vtc, unsigned int noVtc, GLushort* idc, unsigned int noIdc, std::string vtx, std::string frag, bool active, glm::vec3 loc = glm::vec3(0.0), glm::vec3 rot = glm::vec3(0.0));

void vecDraw(Vec* vec);
