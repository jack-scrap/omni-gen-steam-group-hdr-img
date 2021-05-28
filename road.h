#pragma once

#include "obj.h"

typedef struct {
	bool _pass;

	Obj* _parent;

	enum {
		PASS,
		HALT,
		ALERT
	};
} Road;

Road* roadMk(GLfloat* vtc, unsigned int noVtc, GLushort* idc, unsigned int noIdc, bool pass, glm::vec3 loc = glm::vec3(0.0), glm::vec3 rot = glm::vec3(0.0));
