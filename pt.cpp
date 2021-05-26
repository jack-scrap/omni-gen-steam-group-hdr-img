#include <cstdlib>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>

#include "pt.h"
#include "scn.h"

Obj* ptMk(GLfloat* vtx, std::string nameVtx, std::string nameGeom, std::string nameFrag, bool active, glm::vec3 loc, glm::vec3 rot) {
	GLushort idx[1] = {
		0
	};

	return objMk(vtx, 3, idx, 1, nameVtx, nameGeom, nameFrag, active, loc, rot);
}

void ptDraw(Obj* obj) {
	obj->_view = glm::lookAt(cam._pos, cam._pos + glm::vec3(1000.0, -1000.0, 1000.0), glm::vec3(0, 1, 0));
	obj->_view = glm::scale(obj->_view, cam._scale);

	glBindVertexArray(obj->_mesh->_id[Mesh::VAO]);
	obj->_prog.use();

	glUniformMatrix4fv(obj->_uni[Obj::MODEL], 1, GL_FALSE, glm::value_ptr(obj->_acc));
	glUniformMatrix4fv(obj->_uni[Obj::VIEW], 1, GL_FALSE, glm::value_ptr(obj->_view));

	glUniform1ui(obj->_uni[Obj::T], obj->_t);

	glUniform1ui(obj->_uni[Obj::ACTIVE], obj->_active);

	glDrawElements(GL_POINTS, 1, GL_UNSIGNED_SHORT, (GLvoid*) 0);

	obj->_prog.unUse();
	glBindVertexArray(0);

	obj->_t++;
	for (int i = 0; i < obj->_noChild; i++) {
		if (obj->_child[i]) {
			objDraw(obj->_child[i]);
		}
	}
}
