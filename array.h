#pragma once

#include <glm/glm.hpp>

#include "obj.h"
#include "node.h"
#include "idx.h"
#include "math.h"

typedef struct {
	Idx** _data;

	unsigned int _x;
	unsigned int _y;

	float _loc[3];

	Obj* _parent;
} Array;

Array* arrayMk(char* init, unsigned int x, std::string name = "", unsigned int axis = X, glm::vec3 loc = glm::vec3(0.0), glm::vec3 rot = glm::vec3(0.0));

Array* arrayMk(char* init, unsigned int x, unsigned int y, std::string name = "", glm::vec3 loc = glm::vec3(0.0), glm::vec3 rot = glm::vec3(0.0));

void arrayDel(Array* array);

bool arrayEq(Array* lhs, Array* rhs);
