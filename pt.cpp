#include <cstdlib>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>

#include "pt.h"
#include "scn.h"

void ptMv(Pt* pt, GLfloat* d) {
	for (int i = 0; i < 3; i++) {
		pt->_parent->_loc[i] += d[i];
	}

	pt->_parent->_model = glm::scale(pt->_parent->_model, cam._scale);
	pt->_parent->_model = glm::translate(pt->_parent->_model, glm::vec3(d[0], d[1], d[2]));
	pt->_parent->_model = glm::rotate(pt->_parent->_model, (GLfloat) (M_PI / 2), glm::vec3(0.0, 1.0, 0.0));
}

Pt* ptMk(glm::vec3 loc) {
	Pt* _ = (Pt*) malloc(sizeof (Pt));

	GLfloat vtc[3] = {
		0.0, 0.0, 0.0
	};

	GLushort idc[1] = {
		0
	};

	_->_parent = objMk(vtc, sizeof vtc / sizeof *vtc, idc, sizeof idc / sizeof *idc, "tex", "bed", "tex", false, loc);

	_->_parent->_prog.use();

	_->_parent->_prog.unUse();

	return _;
}

void ptDraw(Pt* pt) {
	glBindVertexArray(pt->_parent->_id[VAO]);

	pt->_parent->_prog.use();

	pt->_parent->_model = glm::mat4(1.0);
	pt->_parent->_model = glm::scale(pt->_parent->_model, cam._scale);
	pt->_parent->_model = glm::translate(pt->_parent->_model, pt->_parent->_loc);
	pt->_parent->_model = glm::rotate(pt->_parent->_model, (GLfloat) (M_PI / 2), glm::vec3(0.0, 1.0, 0.0));

	glUniformMatrix4fv(pt->_parent->_uni[MODEL], 1, GL_FALSE, glm::value_ptr(pt->_parent->_model));
	glUniformMatrix4fv(pt->_parent->_uni[VIEW], 1, GL_FALSE, glm::value_ptr(pt->_parent->_view));

	glDrawElements(GL_POINTS, pt->_parent->_noIdc, GL_UNSIGNED_SHORT, (GLvoid*) 0);

	pt->_parent->_prog.unUse();
	glBindVertexArray(0);

	for (int i = 0; i < pt->_parent->_noChild; i++) {
		if (&pt->_parent->_child[i]) {
			objDraw(pt->_parent->_child[i]);
		}
	}
}
