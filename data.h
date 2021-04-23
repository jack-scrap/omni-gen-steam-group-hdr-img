#pragma once

#include <glm/glm.hpp>

#include "obj.h"
#include "node.h"
#include "idx.h"

typedef struct {
	Obj* _parent;

	char* _ptr;
	unsigned int
		_depth,

		_x,
		_y,
		_z;

	float _loc[3];

	Idx** _data;
} Data;

Data* dataMk(char* data, unsigned int sz, std::string name = "", glm::vec3 loc = glm::vec3(0.0, 0.0, 0.0), glm::vec3 rot = glm::vec3(0.0, 0.0, 0.0));

Data* dataMk(char* data, unsigned int x, unsigned int y, std::string name = "", glm::vec3 loc = glm::vec3(0.0, 0.0, 0.0), glm::vec3 rot = glm::vec3(0.0, 0.0, 0.0));

Data* dataMk(char* data, unsigned int x, unsigned int y, unsigned int z, std::string name = "", glm::vec3 loc = glm::vec3(0.0, 0.0, 0.0), glm::vec3 rot = glm::vec3(0.0, 0.0, 0.0));
