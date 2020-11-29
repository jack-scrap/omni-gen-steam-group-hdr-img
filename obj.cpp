#include <stdio.h>
#include <cmath>
#include <thread>
#include <chrono>
#include <GL/glew.h>

#include "obj.h"
#include "scn.h"

Obj objMk(GLfloat *vtc, unsigned int noVtc, GLfloat* loc) {
	Obj* _ = (Obj*) malloc(sizeof (Obj));

	_->_noVtc = noVtc;

	for (int i = 0; i < 3; i++) {
		_->_loc[i] = loc[i];
	}

	glGenVertexArrays(1, &_->_vao);
	glBindVertexArray(_->_vao);

	glGenBuffers(1, &_->_vbo);
	glBindBuffer(GL_ARRAY_BUFFER, _->_vbo);

	glBufferData(GL_ARRAY_BUFFER, _->_noVtc * sizeof (GLfloat), vtc, GL_STATIC_DRAW);

	_->_prog = Prog("main", "solid");

	_->_prog.use();

	_->_attrPos = glGetAttribLocation(_->_prog.id, "pos");
	glVertexAttribPointer(_->_attrPos, 3, GL_FLOAT, GL_FALSE, 0, (GLvoid*) 0);
	glEnableVertexAttribArray(_->_attrPos);

	_->_uniLoc = glGetUniformLocation(_->_prog.id, "loc");

	glUniform3fv(_->_uniLoc, 1, _->_loc);

	return *_;
}

void objDraw(Obj* obj) {
	glBindVertexArray(obj->_vao);
	obj->_prog.use();

	glUniform3fv(obj->_uniLoc, 1, obj->_loc);

	glDrawArrays(GL_TRIANGLES, 0, obj->_noVtc);

	obj->_prog.unUse();
	glBindVertexArray(0);
}
