#include <cstdlib>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>

#include "pt.h"
#include "scn.h"

Obj* ptMk(GLfloat* vtc, GLushort* idc, unsigned int noPrim, std::string nameVtx, std::string nameGeom, std::string nameFrag, bool active, glm::vec3 loc, glm::vec3 rot) {
	return objMk(vtc, idc, sizeof idc / sizeof *idc, nameVtx, nameGeom, nameFrag, active, loc, rot);
}

void ptDraw(Obj* pt) {
	pt->_view = glm::lookAt(cam._pos, cam._pos + glm::vec3(1000.0, -1000.0, 1000.0), glm::vec3(0, 1, 0));
	pt->_view = glm::scale(pt->_view, cam._scale);

	glBindVertexArray(pt->_mesh->_id[Mesh::VAO]);
	pt->_prog.use();

	glUniformMatrix4fv(pt->_uni[Obj::MODEL], 1, GL_FALSE, glm::value_ptr(pt->_acc));
	glUniformMatrix4fv(pt->_uni[Obj::VIEW], 1, GL_FALSE, glm::value_ptr(pt->_view));

	glUniform1ui(pt->_uni[Obj::T], disp->_t);

	glUniform1ui(pt->_uni[Obj::ACTIVE], pt->_active);

	glDrawElements(GL_POINTS, pt->_mesh->_noPrim, GL_UNSIGNED_SHORT, (GLvoid*) 0);

	pt->_prog.unUse();
	glBindVertexArray(0);

	for (int i = 0; i < pt->_noChild; i++) {
		if (pt->_child[i]) {
			objDraw(pt->_child[i]);
		}
	}
}
