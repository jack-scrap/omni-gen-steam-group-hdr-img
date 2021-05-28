#pragma once

#include <glm/glm.hpp>

#include "obj.h"

typedef struct Line {
	static GLushort _idc[2];
} Line;

Obj* lineMk(GLfloat* vtc, std::string nameVtx, std::string nameGeom, std::string nameFrag, bool active, glm::vec3 loc = glm::vec3(0.0), glm::vec3 rot = glm::vec3(0.0));

void lineDraw(Obj* line);
