#pragma once

#include <nlohmann/json.hpp>

#include "obj.h"
#include "var.h"
#include "var.h"

typedef struct {
	Obj* _parent;
} Dict;

Dict* dictMk(nlohmann::json deser, glm::vec3 loc = glm::vec3(0.0), glm::vec3 rot = glm::vec3(0.0));

void dictDel(Dict* dict);
