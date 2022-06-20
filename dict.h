#pragma once

#include "json.hpp"

#include "obj.h"
#include "var.h"
#include "var.h"

typedef struct {
	void** _data;
	unsigned int* _type;
	unsigned int _no;

	float _offset[3];

	Obj* _parent;
} Dict;

Dict* dictMk(nlohmann::json deser, std::string name = "", glm::vec3 loc = glm::vec3(0.0), glm::vec3 rot = glm::vec3(0.0));

void dictDel(Dict* inst);

bool dictEq(Dict* lhs, Dict* rhs);
