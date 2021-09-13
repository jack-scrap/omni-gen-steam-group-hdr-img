#pragma once

#include <glm/glm.hpp>

#include "border.h"
#include "cont.h"

typedef struct {
	unsigned int _i;

	Cont* _data;
	unsigned int _sz;

	Obj* _parent;
} Idx;

Idx* idxMk(unsigned int i, std::string name = "", glm::vec3 loc = glm::vec3(0.0), glm::vec3 rot = glm::vec3(0.0));

Idx* idxMk(unsigned int i, char* c, unsigned int no, std::string name = "", glm::vec3 loc = glm::vec3(0.0), glm::vec3 rot = glm::vec3(0.0));

void idxDel(Idx* idx);

void idxInsert(Idx* idx, Cont* byte);

Cont* idxPop(Idx* idx);

bool idxEq(Idx* lhs, Idx* rhs);
