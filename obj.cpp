#include <stdio.h>
#include <cmath>
#include <thread>
#include <chrono>
#include <vector>
#include <GL/glew.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "obj.h"
#include "scn.h"
#include "util.h"

Obj objMk(GLfloat* vtc, unsigned int noVtc, unsigned short* idc, unsigned int noIdc, GLfloat* loc) {
	// initialize
	Obj* _ = (Obj*) malloc(sizeof (Obj));

	_->_noIdc = noIdc;

	for (int i = 0; i < 3; i++) {
		_->_loc[i] = loc[i];
	}

	// vertex
	glGenVertexArrays(1, &_->_id[VAO]);
	glBindVertexArray(_->_id[VAO]);

	glGenBuffers(1, &_->_id[VBO]);
	glBindBuffer(GL_ARRAY_BUFFER, _->_id[VBO]);
	std::vector<GLfloat> asdf = {
		0.0, 0.0, 0.0,
		1.0, 0.0, 0.0,
		0.0, 1.0, 0.0,
		1.0, 1.0, 0.0
	};
	glBufferData(GL_ARRAY_BUFFER, asdf.size() * sizeof (GLfloat), &vtc[0], GL_STATIC_DRAW);

	glGenBuffers(1, &_->_id[IBO]);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _->_id[IBO]);
	std::vector<GLushort> hjkl = {
		0, 1, 2,
		2, 1, 3
	};
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, hjkl.size() * sizeof (GLushort), &hjkl[0], GL_STATIC_DRAW);

	std::vector<GLushort> qwer = util::mesh::rd::idc("wheel");
	for (const auto& _ : qwer) {
		std::cout << _ << std::endl;
	}

	// matrix
	_->_proj = glm::perspective(glm::radians(45.0f), 800.0f / 600.0f, 0.1f, 100.0f),
	_->_view = glm::lookAt(glm::vec3(3, 3, 3), glm::vec3(0, 0, 0), glm::vec3(0, 1, 0)),
	_->_model = glm::mat4(1.0f);

	_->_prog = Prog("main", "dir");

	_->_prog.use();

	// attribute
	_->_attr[POS] = glGetAttribLocation(_->_prog.id, "pos");
	glVertexAttribPointer(_->_attr[POS], 3, GL_FLOAT, GL_FALSE, 0, (GLvoid*) 0);
	glEnableVertexAttribArray(_->_attr[POS]);

	// uniform
	_->_uni[MODEL] = glGetUniformLocation(_->_prog.id, "model");
	_->_uni[VIEW] = glGetUniformLocation(_->_prog.id, "view");
	_->_uni[PROJ] = glGetUniformLocation(_->_prog.id, "proj");

	_->_uni[LOC] = glGetUniformLocation(_->_prog.id, "loc");

	glUniformMatrix4fv(_->_uni[MODEL], 1, GL_FALSE, glm::value_ptr(_->_model));
	glUniformMatrix4fv(_->_uni[VIEW], 1, GL_FALSE, glm::value_ptr(_->_view));
	glUniformMatrix4fv(_->_uni[PROJ], 1, GL_FALSE, glm::value_ptr(_->_proj));

	glUniform3fv(_->_uni[LOC], 1, _->_loc);

	return *_;
}

void objDraw(Obj* obj) {
	glBindVertexArray(obj->_id[VAO]);
	obj->_prog.use();

	obj->_model = glm::rotate(obj->_model, 0.01f, glm::vec3(0, 1, 0));

	glUniformMatrix4fv(obj->_uni[MODEL], 1, GL_FALSE, glm::value_ptr(obj->_model));
	glUniformMatrix4fv(obj->_uni[VIEW], 1, GL_FALSE, glm::value_ptr(obj->_view));
	glUniformMatrix4fv(obj->_uni[PROJ], 1, GL_FALSE, glm::value_ptr(obj->_proj));

	glUniform3fv(obj->_uni[LOC], 1, obj->_loc);

	glDrawElements(GL_TRIANGLES, obj->_noIdc, GL_UNSIGNED_SHORT, (GLvoid*) 0);

	obj->_prog.unUse();
	glBindVertexArray(0);
}
