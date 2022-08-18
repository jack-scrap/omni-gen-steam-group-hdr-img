#pragma once

#include <glm/glm.hpp>

#include "obj.h"
#include "node.h"
#include "idx.h"
#include "math.h"

typedef struct {
	Obj* _parent;

	Idx** data;

	unsigned int x;
	unsigned int y;
} Array;

Array* arrayMk(char* data, unsigned int x, std::string name = "", unsigned int axis = X, glm::vec3 loc = glm::vec3(0.0), glm::vec3 rot = glm::vec3(0.0));

Array* arrayMk(char* data, unsigned int x, unsigned int y, std::string name = "", glm::vec3 loc = glm::vec3(0.0), glm::vec3 rot = glm::vec3(0.0));

void arrayDel(Array* inst);

void arrayIns(Array* inst, unsigned int x, unsigned int y, Cont* byte);

Cont* arrayRm(Array* inst, unsigned int x, unsigned int y);

bool arrayEq(Array* lhs, Array* rhs);
