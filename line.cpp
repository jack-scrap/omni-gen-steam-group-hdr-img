#include <cstdlib>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "line.h"
#include "obj.h"
#include "scn.h"

Obj* lineMk(GLfloat* vtc, glm::vec3 loc, glm::vec3 rot) {
	GLushort idc[2] = {
		0, 1
	};

	return objMk(vtc, 2 * 3, idc, 2, "obj", "thick", "solid", true, loc, rot);
}

void lineDraw(Obj* line) {
	line->_view = glm::lookAt(cam._pos, cam._pos + glm::vec3(1000.0, -1000.0, 1000.0), glm::vec3(0, 1, 0));
	line->_view = glm::scale(line->_view, cam._scale);

	glBindVertexArray(line->_mesh->_id[Mesh::VAO]);
	line->_prog.use();

	glUniformMatrix4fv(line->_uni[Obj::MODEL], 1, GL_FALSE, glm::value_ptr(line->_acc));
	glUniformMatrix4fv(line->_uni[Obj::VIEW], 1, GL_FALSE, glm::value_ptr(line->_view));

	glUniform1ui(line->_uni[Obj::T], line->_t);

	glUniform1ui(line->_uni[Obj::ACTIVE], line->_active);

	glDrawElements(GL_LINES, 2, GL_UNSIGNED_SHORT, (GLvoid*) 0);

	line->_prog.unUse();
	glBindVertexArray(0);

	line->_t++;
	for (int i = 0; i < line->_noChild; i++) {
		if (line->_child[i]) {
			lineDraw(line->_child[i]);
		}
	}
}
