#include <cstdlib>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "line.h"
#include "obj.h"
#include "scn.h"

Obj* lineMk(GLfloat* vtc, GLushort* idc, unsigned int noPrim, std::string vtx, std::string geom, std::string frag, bool active, glm::vec3 loc, glm::vec3 rot) {
	return objMk(vtc, idc, noPrim, vtx, geom, frag, active, loc, rot);
}

void limDel(Lim* lim) {
	objDel(lim->_parent);

	free(lim);
}
