#pragma once

#include <GL/glew.h>

#include "obj.h"

typedef struct IBeam {
	static GLfloat _vtc[3][2][3];
	static GLushort _idc[3][2];

	GLint _uni[2];

	enum {
		AXIS,
		LN
	};
} IBeam;

Obj* iBeamMk(GLfloat ln, unsigned int axis, glm::vec3 loc = glm::vec3(0.0), glm::vec3 rot = glm::vec3(0.0));
