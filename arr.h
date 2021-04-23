#pragma once

#include <glm/glm.hpp>

#include "obj.h"
#include "node.h"
#include "idx.h"

typedef struct {
	Obj* _parent;

	unsigned int
		_depth,

		_x,
		_y;

	float _loc[3];

	Idx** _data;
} Arr;

Arr* arrMk(char* data, unsigned int sz, std::string name = "", glm::vec3 loc = glm::vec3(0.0, 0.0, 0.0), glm::vec3 rot = glm::vec3(0.0, 0.0, 0.0));

Arr* arrMk(char* data, unsigned int x, unsigned int y, std::string name = "", glm::vec3 loc = glm::vec3(0.0, 0.0, 0.0), glm::vec3 rot = glm::vec3(0.0, 0.0, 0.0));
