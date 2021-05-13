#pragma once

#include <glm/glm.hpp>

#include "border.h"
#include "cont.h"

typedef struct {
	Cont* _data;

	Obj* _parent;
} Idx;

Idx* idxMk(unsigned int i, std::string name = "", glm::vec3 loc = glm::vec3(0.0, 0.0, 0.0), glm::vec3 rot = glm::vec3(0.0, 0.0, 0.0));

Idx* idxMk(unsigned int i, char c, std::string name = "", glm::vec3 loc = glm::vec3(0.0), glm::vec3 rot = glm::vec3(0.0));

void idxInsert(Idx* idx, Cont* byte);

Cont* idxPop(Idx* idx);

bool idxEq(Idx* lhs, Idx* rhs);
