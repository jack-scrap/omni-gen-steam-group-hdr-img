#include <cstdlib>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "line.h"
#include "scn.h"

void lineDraw(Obj* line) {
	line->_view = glm::lookAt(cam._pos, cam._pos + glm::vec3(10000.0, -10000.0, -10000.0), glm::vec3(0, 1, 0));
	line->_view = glm::scale(line->_view, cam._scale);

	glBindVertexArray(line->_mesh->_id[VAO]);
	line->_prog.use();

	glUniformMatrix4fv(line->_uni[MODEL], 1, GL_FALSE, glm::value_ptr(line->_acc));
	glUniformMatrix4fv(line->_uni[VIEW], 1, GL_FALSE, glm::value_ptr(line->_view));

	glUniform1ui(line->_uni[T], line->_t);

	glUniform1ui(line->_uni[ACTIVE], line->_active);

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
