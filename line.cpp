#include <cstdlib>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "line.h"
#include "obj.h"
#include "scn.h"

Obj* lineMk(GLfloat* vtc, GLushort* idc, unsigned int noPrim, std::string nameVtx, std::string nameGeom, std::string nameFrag, bool active, glm::vec3 loc, glm::vec3 rot) {
	return objMk(vtc, idc, noPrim, nameVtx, nameGeom, nameFrag, active, loc, rot);
}
