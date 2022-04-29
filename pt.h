#pragma once

#include <glm/glm.hpp>
#include <string>

#include "obj.h"

Obj* ptMk(GLfloat* vtc, GLushort* idc, unsigned int noPrim, std::string vtx, std::string geom, std::string frag, bool active, glm::vec3 loc = glm::vec3(0.0), glm::vec3 rot = glm::vec3(0.0));
