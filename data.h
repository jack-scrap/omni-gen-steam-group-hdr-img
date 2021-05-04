#pragma once

#include <glm/glm.hpp>

#include "obj.h"
#include "node.h"
#include "idx.h"

typedef struct Var {
	char* _id;

	void* _ptr;
};

typedef struct {
	char* _ptr;
	unsigned int
		_depth,

		_x,
		_y,
		_z;

	float _loc[3];

	Obj* _parent;

	Idx** _data;
} Arr;

Var* varMk(char* name, void* ptr);

Arr* arrMk(char* init, unsigned int sz, std::string name = "", glm::vec3 loc = glm::vec3(0.0, 0.0, 0.0), glm::vec3 rot = glm::vec3(0.0, 0.0, 0.0));

Arr* arrMk(char* init, unsigned int x, unsigned int y, std::string name = "", glm::vec3 loc = glm::vec3(0.0, 0.0, 0.0), glm::vec3 rot = glm::vec3(0.0, 0.0, 0.0));

Arr* arrMk(char* init, unsigned int x, unsigned int y, unsigned int z, std::string name = "", glm::vec3 loc = glm::vec3(0.0, 0.0, 0.0), glm::vec3 rot = glm::vec3(0.0, 0.0, 0.0));
