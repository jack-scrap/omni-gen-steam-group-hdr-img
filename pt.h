#pragma once

#include <glm/glm.hpp>
#include <string>

#include "obj.h"

Obj* ptMk(GLfloat* vtc, GLushort* idc, unsigned int noPrim, std::string nameVtx, std::string nameGeom, std::string nameFrag, bool active, glm::vec3 loc = glm::vec3(0.0), glm::vec3 rot = glm::vec3(0.0));

void ptDraw(Obj* obj);
