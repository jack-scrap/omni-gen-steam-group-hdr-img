#include <cstdlib>
#include <vector>
#include <glm/gtc/type_ptr.hpp>

#include "vec.h"
#include "util.h"
#include "obj.h"
#include "scn.h"

Vec* vecMk(GLfloat* vtc, unsigned int noVtc, GLushort* idc, unsigned int noIdc, std::string vtx, std::string frag, bool active, glm::vec3 loc, glm::vec3 rot) {
	Vec* _ = (Vec*) malloc(sizeof (Vec));

	_->_active = active;
	_->_t = 0;
	_->_loc = glm::translate(glm::mat4(1.0), loc);
	_->_rot = util::matr::rot(glm::mat4(1.0), rot);

	_->_mesh = meshMk(vtc, noVtc * sizeof (GLfloat), idc, noIdc * sizeof (GLushort));

	// matrix
	_->_model = glm::mat4(1.0);
	_->_model *= _->_loc;
	_->_model *= _->_rot;
	_->_view = glm::lookAt(cam._pos + glm::vec3(100), cam._pos, glm::vec3(0, 1, 0));
	_->_proj = glm::ortho(-(state::view[X] / 2.0), state::view[X] / 2.0, -(state::view[Y] / 2.0), state::view[Y] / 2.0, 0.1, 10000.0);

	_->_prog = Prog(vtx, frag);

	_->_prog.use();

	// attribute
	glBindBuffer(GL_ARRAY_BUFFER, _->_mesh->_id[VBO]);
	_->_attr[POS] = glGetAttribLocation(_->_prog.id, "pos");
	glVertexAttribPointer(_->_attr[POS], 2, GL_FLOAT, GL_FALSE, 0, (GLvoid*) 0);
	glEnableVertexAttribArray(_->_attr[POS]);

	// uniform
	_->_uni[MODEL] = glGetUniformLocation(_->_prog.id, "model");
	_->_uni[VIEW] = glGetUniformLocation(_->_prog.id, "view");
	_->_uni[PROJ] = glGetUniformLocation(_->_prog.id, "proj");

	_->_uni[ACTIVE] = glGetUniformLocation(_->_prog.id, "active");

	_->_uni[T] = glGetUniformLocation(_->_prog.id, "t");

	glUniformMatrix4fv(_->_uni[MODEL], 1, GL_FALSE, glm::value_ptr(_->_model));
	glUniformMatrix4fv(_->_uni[VIEW], 1, GL_FALSE, glm::value_ptr(_->_view));
	glUniformMatrix4fv(_->_uni[PROJ], 1, GL_FALSE, glm::value_ptr(_->_proj));

	glUniform1ui(_->_uni[ACTIVE], _->_active);

	glUniform1ui(_->_uni[T], _->_t);

	_->_prog.unUse();
	glBindVertexArray(0);

	return _;
}

void vecDraw(Vec* vec) {
	vec->_view = glm::lookAt(cam._pos + glm::vec3(100), cam._pos, glm::vec3(0, 1, 0));
	vec->_view = glm::scale(vec->_view, cam._scale);

	glBindVertexArray(vec->_mesh->_id[VAO]);
	vec->_prog.use();

	glUniformMatrix4fv(vec->_uni[MODEL], 1, GL_FALSE, glm::value_ptr(vec->_model));
	glUniformMatrix4fv(vec->_uni[VIEW], 1, GL_FALSE, glm::value_ptr(vec->_view));

	glUniform1ui(vec->_uni[T], vec->_t);

	glUniform1ui(vec->_uni[ACTIVE], vec->_active);

	glDrawElements(GL_TRIANGLES, vec->_mesh->_noIdc, GL_UNSIGNED_SHORT, (GLvoid*) 0);

	vec->_prog.unUse();
	glBindVertexArray(0);

	vec->_t++;
}
