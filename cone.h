#pragma once

#include <GL/glew.h>
#include <glm/glm.hpp>

#include "obj.h"

typedef struct Cone {
	GLfloat _bound[2][2];

	GLfloat _loc[3];

	Obj* _parent;

	static GLfloat _vtx[3];
	static GLushort _idx[1];
} Cone;

Cone* coneMk(GLfloat init[2][2], glm::vec3 loc = glm::vec3(0.0), glm::vec3 rot = glm::vec3(0.0));

void coneDel(Cone* cone);
