#pragma once

#include <glm/glm.hpp>

#include "border.h"
#include "cont.h"

typedef struct {
	Obj* _parent;

	Cont** data;
	unsigned int sz;
} Idx;

Idx* idxMk(unsigned int i, std::string name = "", glm::vec3 loc = glm::vec3(0.0), glm::vec3 rot = glm::vec3(0.0));

Idx* idxMk(unsigned int i, char* c, unsigned int sz, std::string name = "", glm::vec3 loc = glm::vec3(0.0), glm::vec3 rot = glm::vec3(0.0));

void idxDel(Idx* inst);

void idxPush(Idx* inst, Cont* byte);

Cont* idxPop(Idx* inst);

bool idxEq(Idx* lhs, Idx* rhs);
