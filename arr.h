#pragma once

#include <glm/glm.hpp>

#include "obj.h"
#include "node.h"

typedef struct {
	Obj* _parent;

	unsigned int
		_depth,

		_x,
		_y;

} Arr;

Arr* arrMk(Node* data, glm::vec3 loc = glm::vec3(0.0, 0.0, 0.0), glm::vec3 rot = glm::vec3(0.0, 0.0, 0.0));
