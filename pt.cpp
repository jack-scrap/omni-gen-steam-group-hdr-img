#include <cstdlib>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "pt.h"

Pt* ptMk() {
	Pt* _ = (Pt*) malloc(sizeof (Pt));

	GLfloat vtc[3] = {
		0.0, 0.0, 0.0
	};

	GLushort idc[1] = {
		0
	};

	_->_parent = objMk(vtc, sizeof vtc / sizeof *vtc, idc, sizeof idc / sizeof *idc, "main", "main", "dir", false, glm::vec3(-1.5, 0.2, 0.0));

	return _;
}

void ptDraw(Pt* pt) {
	glBindVertexArray(pt->_parent->_id[VAO]);

	pt->_parent->_prog.use();

	pt->_parent->_model = glm::mat4(1.0);
	pt->_parent->_model = glm::scale(pt->_parent->_model, glm::vec3(50));
	pt->_parent->_model = glm::translate(pt->_parent->_model, pt->_parent->_loc);

	glUniformMatrix4fv(pt->_parent->_uni[MODEL], 1, GL_FALSE, glm::value_ptr(pt->_parent->_model));
	glUniformMatrix4fv(pt->_parent->_uni[VIEW], 1, GL_FALSE, glm::value_ptr(pt->_parent->_view));
	glUniformMatrix4fv(pt->_parent->_uni[PROJ], 1, GL_FALSE, glm::value_ptr(pt->_parent->_proj));

	glUniform1ui(pt->_parent->_uni[ACTIVE], pt->_parent->_active);

	glDrawElements(GL_POINTS, pt->_parent->_noIdc, GL_UNSIGNED_SHORT, (GLvoid*) 0);

	pt->_parent->_prog.unUse();
	glBindVertexArray(0);

	for (int i = 0; i < pt->_parent->_noChild; i++) {
		if (&pt->_parent->_child[i]) {
			objDraw(pt->_parent->_child[i]);
		}
	}
}
