#pragma once

#include <glm/glm.hpp>

#include "obj.h"
#include "node.h"
#include "idx.h"

typedef struct {
	Idx** _data;

	unsigned int
		_x,
		_y;

	float _loc[3];

	Obj* _parent;
} Arr;

Arr* arrayMk(char* init, unsigned int x, std::string name = "", glm::vec3 loc = glm::vec3(0.0), glm::vec3 rot = glm::vec3(0.0));

Arr* arrayMk(char* init, unsigned int x, unsigned int y, std::string name = "", glm::vec3 loc = glm::vec3(0.0), glm::vec3 rot = glm::vec3(0.0));

bool arrEq(Arr* lhs, Arr* rhs);
