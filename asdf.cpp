#include <stdio.h>
#include <cmath>

#include "asdf.h"
#include "hjkl.h"

Asdf asdfMk(GLfloat* loc) {
	Asdf* _ = (Asdf*) malloc(sizeof (Asdf));

	for (int i = 0; i < 2; i++) {
		_->_loc[i] = loc[i];
	}

	glGenVertexArrays(1, &_->_vao);
	glBindVertexArray(_->_vao);

	glGenBuffers(1, &_->_vbo);
	glBindBuffer(GL_ARRAY_BUFFER, _->_vbo);

	GLfloat vtc[3 * 2];
	float rot = M_PI * 2;
	int n = 3;
	for (int i = 0; i < n * 2; i += 2) {
		float inc = i * (rot / n);

		vtc[i] = sin(inc);
		vtc[i + 1] = cos(inc);
	}
	glBufferData(GL_ARRAY_BUFFER, sizeof vtc, vtc, GL_STATIC_DRAW);

	_->_prog = Prog();

	_->_prog.use();

	_->_attrPos = glGetAttribLocation(_->_prog.id, "pos");
	glVertexAttribPointer(_->_attrPos, 2, GL_FLOAT, GL_FALSE, 0, (GLvoid*) 0);
	glEnableVertexAttribArray(_->_attrPos);

	_->_uniLoc = glGetUniformLocation(_->_prog.id, "loc");

	glUniform2fv(_->_uniLoc, 1, _->_loc);

	return *_;
}

void asdfDraw(Asdf* asdf) {
	glBindVertexArray(asdf->_vao);
	asdf->_prog.use();

	glDrawArrays(GL_TRIANGLES, 0, 3);

	asdf->_prog.unUse();
	glBindVertexArray(0);
}

void set(Asdf* asdf, GLfloat* loc) {
	for (int i = 0; i < 2; i++) {
		asdf->_loc[i] = loc[i];
	}

	asdf->_prog.use();

	glUniform2fv(asdf->_uniLoc, 1, asdf->_loc);

	asdf->_prog.unUse();
}
