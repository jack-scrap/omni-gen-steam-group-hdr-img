#include <cstdlib>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>

#include "pt.h"
#include "scn.h"

Obj* ptMk(GLfloat* vtc, GLushort* idc, unsigned int noPrim, std::string nameVtx, std::string nameGeom, std::string nameFrag, bool active, glm::vec3 loc, glm::vec3 rot) {
	return objMk(vtc, idc, sizeof idc / sizeof *idc, nameVtx, nameGeom, nameFrag, active, loc, rot);
}
