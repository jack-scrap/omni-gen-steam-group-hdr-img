#pragma once

#include <GL/glew.h>

#include "obj.h"

typedef struct IBeam {
	static GLfloat _vtc[3][2][3];
	static GLushort _idc[3][2];
} IBeam;

Obj* iBeamMk(glm::vec3 loc = glm::vec3(0.0), glm::vec3 rot = glm::vec3(0.0));
