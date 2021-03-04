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

	Idx** _data;
} Arr;

Arr* arrMk(Node* data, std::string name = "", glm::vec3 loc = glm::vec3(0.0, 0.0, 0.0), glm::vec3 rot = glm::vec3(0.0, 0.0, 0.0));
