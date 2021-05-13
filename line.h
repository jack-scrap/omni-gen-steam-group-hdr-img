#pragma once

#include <glm/glm.hpp>

#include "obj.h"

Obj* lineMk(GLfloat* vtc, std::string nameVtx, std::string nameGeom, std::string nameFrag, glm::vec3 loc = glm::vec3(0.0), glm::vec3 rot = glm::vec3(0.0));

void lineDraw(Obj* line);
