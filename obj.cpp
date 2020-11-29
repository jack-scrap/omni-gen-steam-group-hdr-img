#include <stdio.h>
#include <cmath>
#include <thread>
#include <chrono>
#include <GL/glew.h>

#include "obj.h"
#include "scn.h"

Obj objMk(GLfloat* vtc, unsigned int noVtc, unsigned short* idc, unsigned int noIdc, GLfloat* loc) {
	Obj* _ = (Obj*) malloc(sizeof (Obj));

	_->_noIdc = noIdc;

	for (int i = 0; i < 3; i++) {
		_->_loc[i] = loc[i];
	}

	glGenVertexArrays(1, &_->_id[VAO]);
	glBindVertexArray(_->_id[VAO]);

	glGenBuffers(1, &_->_id[VBO]);
	glBindBuffer(GL_ARRAY_BUFFER, _->_id[VBO]);
	glBufferData(GL_ARRAY_BUFFER, noVtc * sizeof (GLfloat), vtc, GL_STATIC_DRAW);

	glGenBuffers(1, &_->_id[IBO]);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _->_id[IBO]);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, noIdc * sizeof (GL_UNSIGNED_SHORT), idc, GL_STATIC_DRAW);

	_->_prog = Prog("main", "solid");

	_->_prog.use();

	_->_attr[POS] = glGetAttribLocation(_->_prog.id, "pos");
	glVertexAttribPointer(_->_attr[POS], 3, GL_FLOAT, GL_FALSE, 0, (GLvoid*) 0);
	glEnableVertexAttribArray(_->_attr[POS]);

	_->_uni[LOC] = glGetUniformLocation(_->_prog.id, "loc");

	glUniform3fv(_->_uni[LOC], 1, _->_loc);

	return *_;
}

void objDraw(Obj* obj) {
	glBindVertexArray(obj->_id[VAO]);
	obj->_prog.use();

	glUniform3fv(obj->_uni[LOC], 1, obj->_loc);

	glDrawElements(GL_TRIANGLES, obj->_noIdc, GL_UNSIGNED_SHORT, (GLvoid*) 0);

	obj->_prog.unUse();
	glBindVertexArray(0);
}
