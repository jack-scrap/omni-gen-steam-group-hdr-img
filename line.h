#pragma once

#include <glm/glm.hpp>

#include "obj.h"

enum {
	PASS,
	HALT,
	ALERT
};

Obj* lineMk(GLfloat* vtc, GLushort* idc, unsigned int noPrim, std::string vtx, std::string geom, std::string frag, bool active, glm::vec3 loc = glm::vec3(0.0), glm::vec3 rot = glm::vec3(0.0));
