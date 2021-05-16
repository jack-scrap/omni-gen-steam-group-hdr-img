#pragma once

#include <glm/glm.hpp>

#include "obj.h"

typedef struct {
	bool* _pass;
	unsigned int _no;

	Obj* _parent;
} StreetLight;

StreetLight* streetLightMk(bool* pass, unsigned int no, glm::vec3 loc = glm::vec3(0.0), glm::vec3 rot = glm::vec3(0.0));

extern "C" void streetLightToggle(StreetLight* streetLight, unsigned int i);
