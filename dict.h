#pragma once

#include "json.hpp"

#include "obj.h"
#include "var.h"
#include "var.h"
#include "cont.h"

typedef struct {
	void** data;
	unsigned int* type;
	char** key;
	unsigned int no;

	float offset[3];

	Obj* _parent;
} Dict;

Dict* dictMk(nlohmann::json deser, std::string name = "", glm::vec3 loc = glm::vec3(0.0), glm::vec3 rot = glm::vec3(0.0));

void dictDel(Dict* inst);

int dictHash(Dict* inst, char* key);

void dictPush(Dict* inst, std::string key, unsigned int x, unsigned int y, Cont* byte);

Cont* dictPop(Dict* inst, std::string key, unsigned int x, unsigned int y);

bool dictEq(Dict* lhs, Dict* rhs);
