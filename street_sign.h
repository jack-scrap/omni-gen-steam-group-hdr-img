#pragma once

#include <glm/glm.hpp>

#include "c_buff.h"
#include "obj.h"

typedef struct {
	Obj* _parent;

	CArr _status;
} StreetSign;

StreetSign* streetSignMk(CArr status, glm::vec3 loc = glm::vec3(0.0), glm::vec3 rot = glm::vec3(0.0));

void streetSignDel(StreetSign* inst);

extern "C" void streetSignToggle(StreetSign* inst, unsigned int i);
